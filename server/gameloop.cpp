#include "gameloop.h"

#include <algorithm>
#include <chrono>
#include <set>
#include <string>
#include <utility>

#include "../cmd/server_to_client_gameStarting.h"
#include "../cmd/server_to_client_raceResults.h"
#include "../common/CarStats.h"
#include "../common/constants.h"

#include "Car.h"
#include "City.h"
#include "Lobby.h"
#include "Player.h"
#include "Race.h"

#define FPS 60

// Función helper para convertir nombre del auto a car_type (0-6)
static uint8_t getCarTypeFromName(const std::string& carName) {
    if (carName == "Iveco Daily")
        return 0;
    if (carName == "Dodge Viper")
        return 1;
    if (carName == "Chevrolet Corsa")
        return 2;
    if (carName == "Jeep Wrangler")
        return 3;
    if (carName == "Toyota Tacoma")
        return 4;
    if (carName == "Lincoln TownCar")
        return 5;
    if (carName == "Lamborghini Diablo")
        return 6;
    return 0;  // Default
}

ServerGameLoop::ServerGameLoop(Queue<ClientToServerCmd_Server*>& gameloop_queue,
                               ServerProtectedClients& protected_clients, LobbyStatus& status,
                               struct Lobby* lobby):
        gameloop_queue(gameloop_queue),
        protected_clients(protected_clients),
        status(status),
        lobby(lobby) {}

void ServerGameLoop::process_pending_commands(ServerContext& ctx) {
    try {
        ClientToServerCmd_Server* raw;
        while (gameloop_queue.try_pop(raw)) {
            if (raw) {
                std::unique_ptr<ClientToServerCmd_Server> cmd(raw);
                cmd->execute(ctx);
            }
        }
    } catch (const ClosedQueue&) {
        stop();
        status = LobbyStatus::FINISHED;
    }
}

void ServerGameLoop::update_game_state(Race& race) {
    std::vector<CarSnapshot> snapshot_data = race.getSnapshot();
    auto snapshot_cmd =
            std::make_shared<ServerToClientSnapshot>(snapshot_data, race.getCurrentElapsedTime());
    protected_clients.broadcast(snapshot_cmd);
}

void ServerGameLoop::run() {
    while (should_keep_running()) {
        bool inLobby = true;
        std::set<int> clientsReady;
        std::vector<RaceInfo> racesInfo;
        ServerContext ctx = {.race = nullptr,
                             .client = nullptr,
                             .inLobby = &inLobby,
                             .clientsReady = &clientsReady,
                             .lobby = lobby,
                             .racesInfo = &racesInfo};
        while (should_keep_running() && status == LobbyStatus::WAITING_PLAYERS &&
               clientsReady.size() < protected_clients.size()) {
            process_pending_commands(ctx);
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        std::cout << "Starting Game" << std::endl;
        auto starting_cmd = std::make_shared<ServerToClientGameStarting>();
        protected_clients.broadcast(starting_cmd);
        status = LobbyStatus::IN_GAME;

        // Debug: print car selections
        std::cout << "\n=== CAR SELECTIONS ===" << std::endl;
        if (lobby) {
            std::cout << "Total car selections stored: " << lobby->clientCarSelection.size()
                      << std::endl;
            for (const auto& pair: lobby->clientCarSelection) {
                std::cout << "  Client " << pair.first << " -> " << pair.second << std::endl;
            }
        } else {
            std::cout << "ERROR: lobby is nullptr!" << std::endl;
        }
        std::cout << "Clients ready: " << clientsReady.size() << std::endl;
        // cppcheck-suppress knownEmptyContainer
        for (int clientId: clientsReady) {
            std::cout << "  Client " << clientId << " is ready" << std::endl;
        }
        std::cout << "=====================\n" << std::endl;
        //

        // Create players with their selected cars
        std::vector<std::unique_ptr<Player>> players;

        // First player: from clientsReady (the one who selected a car)
        int playerId = 0;
        // cppcheck-suppress knownEmptyContainer
        for (int clientId: clientsReady) {
            std::string carName = "Iveco Daily";
            if (lobby &&
                lobby->clientCarSelection.find(clientId) != lobby->clientCarSelection.end()) {
                carName = lobby->clientCarSelection[clientId];
                std::cout << "Found car selection for client " << clientId << ": " << carName
                          << std::endl;
            } else {
                std::cout << "No car selection found for client " << clientId << ", using default"
                          << std::endl;
            }
            std::cout << "Creating player " << playerId << " (client " << clientId
                      << ") with car: " << carName << std::endl;

            // Use generic stats for now (can be customized per car later)
            CarStats stats = CarStatsDatabase::getCarStats(carName);
            std::cout << "  Car stats - Accel: " << stats.acceleration
                      << ", Max Speed: " << stats.max_speed << ", Turn Speed: " << stats.turn_speed
                      << ", Mass: " << stats.mass << ", Brake Force: " << stats.brake_force
                      << ", Handling: " << stats.handling << ", Health Max: " << stats.health_max
                      << ", Length: " << stats.length << ", Width: " << stats.width << std::endl;
            uint8_t car_type = getCarTypeFromName(carName);
            players.emplace_back(std::make_unique<Player>(carName, clientId, stats, car_type));
            playerId++;
        }

        // cppcheck-suppress knownEmptyContainer
        for (auto& raceInfo: racesInfo) {
            Race race(raceInfo.city, raceInfo.trackFile, players);
            ctx = {.race = &race,
                   .client = nullptr,
                   .inLobby = &inLobby,
                   .clientsReady = &clientsReady,
                   .lobby = lobby,
                   .racesInfo = nullptr};

            auto startingRaceCmd =
                    std::make_shared<ServerToClientStartingRace>(raceInfo.city, raceInfo.trackFile);
            protected_clients.broadcast(startingRaceCmd);

            try {
                ClientToServerCmd_Server* raw;
                while (gameloop_queue.try_pop(raw)) {}
            } catch (const ClosedQueue&) {
                stop();
                status = LobbyStatus::FINISHED;
                break;
            }

            race.start();

            const std::chrono::milliseconds frameDuration(1000 / FPS);
            auto t1 = std::chrono::high_resolution_clock::now();
            bool resultsAlreadySent = false;
            std::set<int> playersWhoAlreadyReceivedPartial;

            while (!race.isFinished() && should_keep_running()) {
                process_pending_commands(ctx);

                race.updatePhysics(std::chrono::duration<float>(frameDuration).count());

                send_partial_results(race, playersWhoAlreadyReceivedPartial);
                update_game_state(race);

                auto t2 = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
                if (elapsed < frameDuration) {
                    std::this_thread::sleep_for(frameDuration - elapsed);
                    t1 += frameDuration;
                } else {
                    auto lostFrames = elapsed / frameDuration;
                    t1 += lostFrames * frameDuration;
                }
            }
            if (race.isFinished() && !resultsAlreadySent) {
                send_acumulated_results(race, players, resultsAlreadySent);
            }
        }

        stop();
        status = LobbyStatus::FINISHED;
    }
}


void ServerGameLoop::send_partial_results(Race& race,
                                          std::set<int>& playersWhoAlreadyReceivedPartial) {
    for (const auto& [pid, finishTime]: race.getFinishTimes()) {
        if (playersWhoAlreadyReceivedPartial.count(pid))
            continue;

        playersWhoAlreadyReceivedPartial.insert(pid);
        std::vector<PlayerResult> partial;
        std::string playerName = "Unknown";

        auto it = std::find_if(race.getPlayers().begin(), race.getPlayers().end(),
                               [pid](const auto& p) { return p->getId() == pid; });

        if (it != race.getPlayers().end()) {
            playerName = (*it)->getName();
        }

        uint8_t position = (finishTime < 0 ? 0 : playersWhoAlreadyReceivedPartial.size());

        float upgradePenalty = 0.0f;
        auto it_ = std::find_if(race.getPlayers().begin(), race.getPlayers().end(),
                                [pid](const auto& p) { return p->getId() == pid; });

        if (it_ != race.getPlayers().end()) {
            upgradePenalty = (*it_)->getCarUpgrades().getTimePenalty();
        }

        partial.emplace_back(pid, playerName, finishTime + upgradePenalty, position);

        auto partialCmd = std::make_shared<ServerToClientRaceResults>(partial, false);

        protected_clients.broadcast(partialCmd);

        std::cout << "[RACE] Sent PARTIAL result to player " << pid << " => " << finishTime
                  << " seconds\n";
    }
}

void ServerGameLoop::send_acumulated_results(Race& race,
                                             std::vector<std::unique_ptr<Player>> const& players,
                                             bool& resultsAlreadySent) {
    resultsAlreadySent = true;
    const auto& finishTimes = race.getFinishTimes();
    std::vector<std::pair<int, float>> pairs;
    for (const auto& [pid, ftime]: finishTimes) pairs.emplace_back(pid, ftime);
    std::sort(pairs.begin(), pairs.end(), [](const auto& a, const auto& b) {
        float ta = a.second;
        float tb = b.second;

        if (ta < 0 && tb < 0)
            return false;
        if (ta < 0)
            return false;
        if (tb < 0)
            return true;

        return ta < tb;
    });
    std::vector<PlayerResult> fullResults;
    for (size_t i = 0; i < pairs.size(); i++) {
        int playerId_ = pairs[i].first;
        float finishTime = pairs[i].second;
        std::string playerName = "Unknown";
        for (const auto& p: players)
            if (p->getId() == playerId_)
                playerName = p->getName();

        float upgradePenalty = 0.0f;

        auto it = std::find_if(players.begin(), players.end(),
                               [playerId_](const auto& p) { return p->getId() == playerId_; });

        if (it != players.end()) {
            playerName = (*it)->getName();
            upgradePenalty = (*it)->getCarUpgrades().getTimePenalty();
        }

        fullResults.emplace_back((uint8_t)playerId_, playerName, finishTime + upgradePenalty,
                                 (uint8_t)(i + 1));
    }
    auto fullCmd = std::make_shared<ServerToClientRaceResults>(fullResults, true);
    protected_clients.broadcast(fullCmd);
    std::cout << "[RACE] Sent FULL results to all players.\n";

    for (const auto& p: players) {
        int pid = p->getId();

        auto it = std::find_if(
                accumulatedResults.begin(), accumulatedResults.end(),
                [pid](const AccumulatedResultDTO& dto) { return dto.playerId == pid; });

        if (it == accumulatedResults.end()) {
            accumulatedResults.push_back({pid, 0, -1.0f});
        }
    }

    for (auto& [pid, ftime]: finishTimes) {

        if (ftime < 0)
            continue;

        auto it = std::find_if(
                accumulatedResults.begin(), accumulatedResults.end(),
                [pid](const AccumulatedResultDTO& dto) { return dto.playerId == pid; });

        it->completedRaces += 1;

        if (it->totalTime < 0)
            it->totalTime = 0;

        it->totalTime += ftime;
    }

    std::vector<AccumulatedResultDTO> orderedAccum = accumulatedResults;

    std::sort(orderedAccum.begin(), orderedAccum.end(),
              [](const AccumulatedResultDTO& a, const AccumulatedResultDTO& b) {
                  if (a.completedRaces != b.completedRaces)
                      return a.completedRaces > b.completedRaces;

                  if (a.totalTime != b.totalTime)
                      return a.totalTime < b.totalTime;

                  return a.playerId < b.playerId;
              });

    auto accumCmd = std::make_shared<ServerToClientAccumulatedResults>(orderedAccum);
    protected_clients.broadcast(accumCmd);
    std::cout << "\n--- ACUMULADO HASTA AHORA ---\n";
    for (const auto& acc: accumulatedResults) {
        int pid = acc.playerId;
        std::cout << "Player " << pid << ": completed=" << acc.completedRaces
                  << ", totalTime=" << acc.totalTime << "\n";
    }
    std::this_thread::sleep_for(std::chrono::seconds(10));
}
