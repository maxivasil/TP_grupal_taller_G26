#include "gameloop.h"

#include <chrono>
#include <set>
#include <string>

#include "../common/constants.h"
#include "../cmd/server_to_client_gameStarting.h"

#include "Car.h"
#include "City.h"
#include "Player.h"
#include "Race.h"

#define FPS 60

ServerGameLoop::ServerGameLoop(Queue<ClientToServerCmd_Server*>& gameloop_queue,
                               ServerProtectedClients& protected_clients, LobbyStatus& status):
        gameloop_queue(gameloop_queue), protected_clients(protected_clients), status(status) {}

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
                             .clientsReady = &clientsReady};
        while (should_keep_running() && status == LobbyStatus::WAITING_PLAYERS &&
               clientsReady.size() < protected_clients.size()) {
            process_pending_commands(ctx);
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        std::cout << "Starting Game" << std::endl;
        auto starting_cmd = std::make_shared<ServerToClientGameStarting>();
        protected_clients.broadcast(starting_cmd);
        status = LobbyStatus::IN_GAME;
        CarStats statsA = {.acceleration = 20.0f,
                           .max_speed = 100.0f,
                           .turn_speed = 5.0f,
                           .mass = 1200.0f,
                           .brake_force = 15.0f,
                           .handling = 1.8f,
                           .health_max = 100.0f,
                           .length = 4.4f,
                           .width = 2.5714f};

        CarStats statsB = {.acceleration = 20.0f,
                           .max_speed = 100.0f,
                           .turn_speed = 5.0f,
                           .mass = 1200.0f,
                           .brake_force = 15.0f,
                           .handling = 1.8f,
                           .health_max = 100.0f,
                           .length = 4.4f,
                           .width = 2.5714f};

        std::string trackFile = "tracks/track.yaml";
        std::vector<std::unique_ptr<Player>> players;
        players.emplace_back(std::make_unique<Player>("A", 0, statsA));
        players.emplace_back(std::make_unique<Player>("B", 1, statsB));
        Race race(CityName::LibertyCity, trackFile, players);
        ctx = {.race = &race,
               .client = nullptr,
               .inLobby = &inLobby,
               .clientsReady = &clientsReady};
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
