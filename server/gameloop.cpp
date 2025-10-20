#include "gameloop.h"

#include <chrono>

#include "../common/constants.h"

ServerGameLoop::ServerGameLoop(Queue<int>& gameloop_queue,
                               ServerProtectedClients& protected_clients):
        gameloop_queue(gameloop_queue), protected_clients(protected_clients), actives_nitro() {}

void ServerGameLoop::try_pop_and_activate_nitro() {
    int client_id;
    while (gameloop_queue.try_pop(client_id)) {
        if (actives_nitro.nitro_timers[client_id] == 0) {
            actives_nitro.nitro_timers[client_id] = nitro_duration_loops;
            actives_nitro.count++;
            protected_clients.broadcast_message(actives_nitro.count, INFORM_NITRO_ACTIVATED);
        }
    }
}

void ServerGameLoop::decrement_nitro_timers() {
    for (auto& [id, timer]: actives_nitro.nitro_timers) {
        if (timer > 0) {
            timer--;
            if (timer == 0) {
                actives_nitro.count--;
                protected_clients.broadcast_message(actives_nitro.count, INFORM_NITRO_EXPIRED);
            }
        }
    }
}

void ServerGameLoop::run() {
    while (should_keep_running()) {
        try_pop_and_activate_nitro();
        decrement_nitro_timers();
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}
