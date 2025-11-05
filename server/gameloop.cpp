#include "gameloop.h"

#include <chrono>
#include <string>

#include "../common/constants.h"

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
        bool inLobby = true;
        ServerContext ctx = {.race = &race, .client = nullptr, .inLobby = &inLobby};
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
        stop();
        /*


        const float timeStep = 1.0f / 60.0f;
        // 400 2400  -- ----- 57.3, 308.6
        // 580 2400  -------- 83.1, 308.6
        // 62 828    -------- 8.9   106.5

        for (int i = 0; i < 180; ++i) {
            for (auto& p: players) {
                p->accelerate();
            }

            race.updatePhysics(timeStep);

            b2Vec2 posA = players[0]->getPosition();
            b2Vec2 posB = players[1]->getPosition();

            std::cout << "Step " << i << ":\n";
            std::cout << "  CarA Pos (" << posA.x << ", " << posA.y
            << ") HP=" << players[0]->getCurrentHealth() << "\n";
            std::cout << "  CarB Pos (" << posB.x << ", " << posB.y
            << ") HP=" << players[0]->getCurrentHealth() << "\n\n";
        }
        stop();
        */
    }
}
