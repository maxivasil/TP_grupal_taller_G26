#include "gameloop.h"

#include <chrono>

#include "../common/constants.h"

ServerGameLoop::ServerGameLoop(Queue<ClientToServerCmd_Server*>& gameloop_queue,
                               ServerProtectedClients& protected_clients):
        gameloop_queue(gameloop_queue), protected_clients(protected_clients) {}

void ServerGameLoop::process_pending_commands() {
    ClientToServerCmd_Server* cmd = nullptr;
    while (gameloop_queue.try_pop(cmd)) {
        if (cmd) {
            cmd->execute();
            delete cmd;
        }
    }
}

void ServerGameLoop::update_game_state() {
    std::vector<CarSnapshot> snapshot_data = {
            {1, 10.0f, 15.5f, false, 100.0f, 2.0f, 0},
            {2, 20.5f, 25.0f, false, 90.0f, 2.5f, 1},
    };

    // Crear comando Snapshot
    auto snapshot_cmd = new ServerToClientSnapshot(snapshot_data);

    // Broadcast a todos los clientes conectados
    protected_clients.broadcast(snapshot_cmd);

    // IMPORTANTE: el broadcast hace una copia por cliente, así que liberamos el original
    delete snapshot_cmd;
}

void ServerGameLoop::run() {
    while (should_keep_running()) {
        process_pending_commands();
        update_game_state();
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}
