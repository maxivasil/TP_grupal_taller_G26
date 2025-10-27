#include "gameloop.h"

#include <chrono>

#include "../common/constants.h"

ServerGameLoop::ServerGameLoop(Queue<ClientToServerCmd_Server*>& gameloop_queue,
                               ServerProtectedClients& protected_clients):
        gameloop_queue(gameloop_queue), protected_clients(protected_clients), actives_nitro() {}

void ServerGameLoop::process_pending_commands() {
    ClientToServerCmd_Server* cmd = nullptr;
    while (gameloop_queue.try_pop(cmd)) {
        if (cmd) {
            cmd->execute();
            delete cmd;
        }
    }
}

void ServerGameLoop::update_game_state() {}

void ServerGameLoop::run() {
    while (should_keep_running()) {
        process_pending_commands();
        update_game_state();
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}
