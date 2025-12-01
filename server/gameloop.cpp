#include "gameloop.h"

#include <algorithm>
#include <chrono>
#include <set>
#include <string>
#include <utility>

#include "../common/CarStats.h"
#include "../common/constants.h"
#include "cmd/server_to_client_gameStarting_server.h"
#include "cmd/server_to_client_raceResults_server.h"
#include "game_logic/Car.h"
#include "game_logic/City.h"
#include "game_logic/Player.h"
#include "game_logic/Race.h"

#include "Lobby.h"

#define FPS 60

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

void ServerGameLoop::update_game_state(const Race& race) {
    std::vector<CarSnapshot> snapshot_data = race.getSnapshot();
    auto snapshot_cmd = std::make_shared<ServerToClientSnapshot_Server>(
            snapshot_data, race.getCurrentElapsedTime());

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
                             .racesInfo = &racesInfo,
                             .players = nullptr};
        while (should_keep_running() && status == LobbyStatus::WAITING_PLAYERS &&
               clientsReady.size() < protected_clients.size()) {
            process_pending_commands(ctx);
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        auto starting_cmd = std::make_shared<ServerToClientGameStarting_Server>();
        protected_clients.broadcast(starting_cmd);
        status = LobbyStatus::IN_GAME;

        // Create players with their selected cars
        std::vector<std::unique_ptr<Player>> players;

        // First player: from clientsReady (the one who selected a car)
        uint32_t playerId = 0;
        // cppcheck-suppress knownEmptyContainer
        for (uint32_t clientId: clientsReady) {
            std::string userName;
            std::string carName = "Iveco Daily";
            if (lobby &&
                lobby->clientCarSelection.find(clientId) != lobby->clientCarSelection.end()) {
                carName = lobby->clientCarSelection[clientId];
            }

            // Use generic stats for now (can be customized per car later)
            CarStats stats = CarStatsDatabase::getCarStats(carName);
            if (lobby) {
                userName = lobby->clientUsernames[clientId];
            }
            uint8_t car_type = CarStatsDatabase::getCarTypeFromName(carName);
            players.emplace_back(std::make_unique<Player>(userName, clientId, stats, car_type));
            playerId++;
        }

        int raceNumber = 0;
        // cppcheck-suppress knownEmptyContainer
        for (auto& raceInfo: racesInfo) {
            Race race(raceInfo.city, raceInfo.trackFile, players);
            ctx = {.race = &race,
                   .client = nullptr,
                   .inLobby = &inLobby,
                   .clientsReady = &clientsReady,
                   .lobby = lobby,
                   .racesInfo = nullptr,
                   .players = &players};

            if (raceNumber != 0) {
                handle_upgrades(players);
            }

            auto startingRaceCmd = std::make_shared<ServerToClientStartingRace_Server>(
                    raceInfo.city, raceInfo.trackFile, raceNumber == racesInfo.size() - 1,
                    UINT8_MAX);
            protected_clients.broadcast(startingRaceCmd);

            int secondsDuration = (raceNumber == 0) ? 15 : 5;
            const std::chrono::milliseconds countdownFrame(1000);

            auto countdownStart = std::chrono::high_resolution_clock::now();
            int countdownValue = secondsDuration;

            race.setPausePhysics(true);

            while (countdownValue >= 0 && should_keep_running()) {
                auto now = std::chrono::high_resolution_clock::now();
                auto elapsed =
                        std::chrono::duration_cast<std::chrono::milliseconds>(now - countdownStart);

                int nextCountdownValue =
                        secondsDuration - (elapsed.count() / countdownFrame.count());

                if (nextCountdownValue < countdownValue) {
                    countdownValue = nextCountdownValue;
                    auto countdownCmd = std::make_shared<ServerToClientStartingRace_Server>(
                            raceInfo.city, raceInfo.trackFile, raceNumber == racesInfo.size() - 1,
                            (uint8_t)countdownValue);
                    protected_clients.broadcast(countdownCmd);
                }

                update_game_state(race);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            try {
                empty_gameloop_queue();
            } catch (const ClosedQueue&) {
                stop();
                status = LobbyStatus::FINISHED;
                break;
            }
            race.setPausePhysics(false);

            auto realStartTime = std::chrono::steady_clock::now();
            race.start(realStartTime);

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
            raceNumber++;
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

        auto partialCmd = std::make_shared<ServerToClientRaceResults_Server>(partial, false);

        protected_clients.broadcast(partialCmd);
    }
}

void ServerGameLoop::send_acumulated_results(const Race& race,
                                             std::vector<std::unique_ptr<Player>> const& players,
                                             bool& resultsAlreadySent) {
    resultsAlreadySent = true;
    const auto& finishTimes = race.getFinishTimes();
    std::vector<std::pair<int, float>> sortedFinishTimes(finishTimes.begin(), finishTimes.end());

    std::sort(sortedFinishTimes.begin(), sortedFinishTimes.end(), [](const auto& a, const auto& b) {
        float ta = a.second, tb = b.second;
        if (ta < 0 && tb < 0)
            return false;
        if (ta < 0)
            return false;
        if (tb < 0)
            return true;
        return ta < tb;
    });

    std::vector<PlayerResult> fullResults;
    for (size_t i = 0; i < sortedFinishTimes.size(); ++i) {
        int playerId = sortedFinishTimes[i].first;
        float finishTime = sortedFinishTimes[i].second;

        auto it = std::find_if(players.begin(), players.end(),
                               [playerId](const auto& p) { return p->getId() == playerId; });

        std::string playerName = (it != players.end()) ? (*it)->getName() : "Unknown";
        float upgradePenalty =
                (it != players.end()) ? (*it)->getCarUpgrades().getTimePenalty() : 0.0f;

        fullResults.emplace_back(static_cast<uint8_t>(playerId), playerName,
                                 finishTime + upgradePenalty, static_cast<uint8_t>(i + 1));
    }
    auto fullCmd = std::make_shared<ServerToClientRaceResults_Server>(fullResults, true);
    protected_clients.broadcast(fullCmd);

    for (const auto& p: players) {
        auto it = std::find_if(accumulatedResults.begin(), accumulatedResults.end(),
                               [pid = p->getId()](const AccumulatedResultDTO& dto) {
                                   return dto.playerId == pid;
                               });

        if (it == accumulatedResults.end()) {
            accumulatedResults.push_back({p->getId(), p->getName(), 0, -1.0f});
        }
    }

    for (const auto& playerResult: fullResults) {
        if (playerResult.finishTime < 0)
            continue;

        auto it = std::find_if(accumulatedResults.begin(), accumulatedResults.end(),
                               [playerResult](const AccumulatedResultDTO& dto) {
                                   return dto.playerId == playerResult.playerId;
                               });

        if (it != accumulatedResults.end()) {
            it->completedRaces += 1;
            if (it->totalTime < 0)
                it->totalTime = 0;
            it->totalTime += playerResult.finishTime;
        }
    }
    auto orderedAccum = accumulatedResults;
    std::sort(orderedAccum.begin(), orderedAccum.end(),
              [](const AccumulatedResultDTO& a, const AccumulatedResultDTO& b) {
                  if (a.completedRaces != b.completedRaces)
                      return a.completedRaces > b.completedRaces;
                  if (a.totalTime != b.totalTime)
                      return a.totalTime < b.totalTime;
                  return a.playerId < b.playerId;
              });

    auto accumCmd = std::make_shared<ServerToClientAccumulatedResults_Server>(orderedAccum);
    protected_clients.broadcast(accumCmd);
}

void ServerGameLoop::handle_upgrades(std::vector<std::unique_ptr<Player>>& players) {
    auto start_time = std::chrono::high_resolution_clock::now();
    ServerContext ctx = {.players = &players};
    while (should_keep_running()) {
        auto current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_time = current_time - start_time;
        if (elapsed_time.count() >= 10.0) {
            break;
        }

        process_pending_commands(ctx);

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void ServerGameLoop::empty_gameloop_queue() {
    ClientToServerCmd_Server* raw;
    while (gameloop_queue.try_pop(raw)) {
        std::unique_ptr<ClientToServerCmd_Server> cmd(raw);
    }
}
