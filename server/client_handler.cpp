#include "client_handler.h"

#include <utility>

#include "../common/constants.h"


ServerClientHandler::ServerClientHandler(int client_id, Socket&& s, Queue<int>& gameloop_queue):
        client_id(client_id),
        protocol(std::move(s)),
        send_queue(),
        receiver(client_id, protocol, gameloop_queue),
        sender(protocol, send_queue) {}

void ServerClientHandler::run() {
    try {
        receiver.start();
        sender.start();

        receiver.join();
        sender.join();
        protocol.close_connection();
    } catch (const std::exception& e) {
        if (!should_keep_running()) {
            if (receiver.is_alive()) {
                receiver.join();
            }
            if (sender.is_alive()) {
                sender.join();
            }
            return;
        }
        std::cerr << "Unexpected exception in ClientHandler" << e.what() << std::endl;
    }
}

void ServerClientHandler::stop() {
    receiver.stop();
    send_queue.close();
    sender.stop();
    Thread::stop();
}

bool ServerClientHandler::is_dead() const {
    return !should_keep_running() || protocol.is_connection_closed();
}

void ServerClientHandler::send_message(int cars_with_nitro, int msg) {
    std::vector<int> message = {cars_with_nitro, msg};
    send_queue.push(message);
}
