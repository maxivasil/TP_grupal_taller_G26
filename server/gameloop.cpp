#include "gameloop.h"

#include <chrono>
#include <set>
#include <string>

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
        gameloop_queue(gameloop_queue), protected_clients(protected_clients), status(status), lobby(lobby) {}

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
        // HARCODEADO (luego modificar y eliminar)
        bool inLobby = true;
        std::set<int> clientsReady;
        ServerContext ctx = {.race = nullptr,
                             .client = nullptr,
                             .inLobby = &inLobby,
                             .clientsReady = &clientsReady,
                             .lobby = lobby};
        while (should_keep_running() && status == LobbyStatus::WAITING_PLAYERS &&
               clientsReady.size() < protected_clients.size()) {
            process_pending_commands(ctx);
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            // Seguramente necesite broadcastear el estado del lobby a los clientes (para que lo
            // puedan mostrar)
        }
        status = LobbyStatus::IN_GAME;
        
        // Debug: print car selections
        std::cout << "\n=== CAR SELECTIONS ===" << std::endl;
        if (lobby) {
            std::cout << "Total car selections stored: " << lobby->clientCarSelection.size() << std::endl;
            for (const auto& pair : lobby->clientCarSelection) {
                std::cout << "  Client " << pair.first << " -> " << pair.second << std::endl;
            }
        } else {
            std::cout << "ERROR: lobby is nullptr!" << std::endl;
        }
        std::cout << "Clients ready: " << clientsReady.size() << std::endl;
        for (int clientId : clientsReady) {
            std::cout << "  Client " << clientId << " is ready" << std::endl;
        }
        std::cout << "=====================\n" << std::endl;
        
        // Create players with their selected cars
        std::vector<std::unique_ptr<Player>> players;
        
        // First player: from clientsReady (the one who selected a car)
        int playerId = 0;
        for (int clientId : clientsReady) {
            std::string carName = "DefaultCar";
            if (lobby && lobby->clientCarSelection.find(clientId) != lobby->clientCarSelection.end()) {
                carName = lobby->clientCarSelection[clientId];
                std::cout << "Found car selection for client " << clientId << ": " << carName << std::endl;
            } else {
                std::cout << "No car selection found for client " << clientId << ", using default" << std::endl;
            }
            std::cout << "Creating player " << playerId << " (client " << clientId << ") with car: " << carName << std::endl;
            
            // Use generic stats for now (can be customized per car later)
            CarStats stats = {.acceleration = 20.0f,
                             .max_speed = 100.0f,
                             .turn_speed = 5.0f,
                             .mass = 1200.0f,
                             .brake_force = 15.0f,
                             .handling = 1.8f,
                             .health_max = 100.0f,
                             .length = 4.4f,
                             .width = 2.5714f};
            players.emplace_back(std::make_unique<Player>(carName, clientId, stats));
            playerId++;
        }
        
        // Second player: always create a second player with a default car for racing
        if (players.size() < 2) {
            std::cout << "Adding second player with default car" << std::endl;
            CarStats statsB = {.acceleration = 20.0f,
                              .max_speed = 100.0f,
                              .turn_speed = 5.0f,
                              .mass = 1200.0f,
                              .brake_force = 15.0f,
                              .handling = 1.8f,
                              .health_max = 100.0f,
                              .length = 4.4f,
                              .width = 2.5714f};
            players.emplace_back(std::make_unique<Player>("DefaultCar", 999, statsB));
        }
        
        std::string trackFile = "tracks/track.yaml";
        Race race(CityName::LibertyCity, trackFile, players);
        ctx = {.race = &race,
               .client = nullptr,
               .inLobby = &inLobby,
               .clientsReady = &clientsReady,
               .lobby = lobby};
        race.start();

        const std::chrono::milliseconds frameDuration(1000 / FPS);
        auto t1 = std::chrono::high_resolution_clock::now();
        while (!race.isFinished() && should_keep_running()) {
            process_pending_commands(ctx);

            race.updatePhysics(std::chrono::duration<float>(frameDuration).count());

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
        status = LobbyStatus::FINISHED;
        stop();
    }
}
