#include "gameloop.h"

#include <algorithm>
#include <chrono>
#include <set>
#include <string>
#include <utility>

#include "../cmd/server_to_client_gameStarting.h"
#include "../cmd/server_to_client_raceResults.h"
#include "../common/constants.h"

#include "Car.h"
#include "City.h"
#include "Lobby.h"
#include "Player.h"
#include "Race.h"

#define FPS 60

ServerGameLoop::ServerGameLoop(Queue<ClientToServerCmd_Server*>& gameloop_queue,
                               ServerProtectedClients& protected_clients, LobbyStatus& status,
                               struct Lobby* lobby):
        gameloop_queue(gameloop_queue),
        protected_clients(protected_clients),
        status(status),
        lobby(lobby) {}

void ServerGameLoop::process_pending_commands(ServerContext& ctx) {
    ClientToServerCmd_Server* raw;
    while (gameloop_queue.try_pop(raw)) {
        if (raw) {
            std::unique_ptr<ClientToServerCmd_Server> cmd(raw);
            cmd->execute(ctx);
        }
    }
}

void ServerGameLoop::update_game_state(Race& race) {
    std::vector<CarSnapshot> snapshot_data = race.getSnapshot();

    // Crear comando Snapshot
    auto snapshot_cmd = std::make_shared<ServerToClientSnapshot>(snapshot_data);

    // Broadcast a todos los clientes conectados
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

        // Create players with their selected cars
        std::vector<std::unique_ptr<Player>> players;

        // First player: from clientsReady (the one who selected a car)
        int playerId = 0;
        // cppcheck-suppress knownEmptyContainer
        for (int clientId: clientsReady) {
            std::string carName = "DefaultCar";
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
            CarStats stats = {.acceleration = 20.0f,
                              .max_speed = 120.0f,
                              .turn_speed = 7.0f,
                              .mass = 1200.0f,
                              .brake_force = 15.0f,
                              .handling = 2.8f,
                              .health_max = 100.0f,
                              .length = 4.011f,
                              .width = 2.8288f};
            players.emplace_back(std::make_unique<Player>(carName, clientId, stats));
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

            auto startingRaceCmd = std::make_shared<ServerToClientStartingRace>();
            protected_clients.broadcast(startingRaceCmd);

            ClientToServerCmd_Server* raw;
            while (gameloop_queue.try_pop(raw)) {}

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

        uint8_t position = playersWhoAlreadyReceivedPartial.size();

        partial.emplace_back(static_cast<uint8_t>(pid), playerName,
                             finishTime >= 0 ? finishTime : 0.0f, position);

        auto partialCmd = std::make_shared<ServerToClientRaceResults>(partial, false);

        protected_clients.broadcast(partialCmd);

        std::cout << "[RACE] Sent PARTIAL result to player " << pid << " => " << finishTime
                  << " seconds\n";
    }
}

void ServerGameLoop::send_acumulated_results(Race& race,
                                             std::vector<std::unique_ptr<Player>>& players,
                                             bool& resultsAlreadySent) {
    resultsAlreadySent = true;
    const auto& finishTimes = race.getFinishTimes();
    std::vector<std::pair<int, float>> pairs;
    for (const auto& [pid, ftime]: finishTimes) pairs.emplace_back(pid, ftime);
    std::sort(pairs.begin(), pairs.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });
    std::vector<PlayerResult> fullResults;
    for (size_t i = 0; i < pairs.size(); i++) {
        int playerId_ = pairs[i].first;
        float finishTime = pairs[i].second;
        std::string playerName = "Unknown";
        for (const auto& p: race.getPlayers())
            if (p->getId() == playerId_)
                playerName = p->getName();
        fullResults.emplace_back((uint8_t)playerId_, playerName, finishTime, (uint8_t)(i + 1));
    }
    auto fullCmd = std::make_shared<ServerToClientRaceResults>(fullResults, true);
    protected_clients.broadcast(fullCmd);
    std::cout << "[RACE] Sent FULL results to all players.\n";
    // === NUEVO: ACUMULAR RESULTADOS DE ESTA CARRERA ===
    for (auto& p: players) {
        int pid = p->getId();
        // Ver si terminó la carrera
        auto it = race.getFinishTimes().find(pid);
        if (it != race.getFinishTimes().end()) {
            float time = it->second;
            if (time >= 0.0f) {
                accumulatedResults[pid].completedRaces++;
                accumulatedResults[pid].totalTime += time;
            }
        }
    }
    auto accumCmd = std::make_shared<ServerToClientAccumulatedResults>(accumulatedResults);
    protected_clients.broadcast(accumCmd);
    std::cout << "\n--- ACUMULADO HASTA AHORA ---\n";
    for (auto& [pid, acc]: accumulatedResults) {
        std::cout << "Player " << pid << ": completed=" << acc.completedRaces
                  << ", totalTime=" << acc.totalTime << "\n";
    }
    std::this_thread::sleep_for(std::chrono::seconds(10));
}
