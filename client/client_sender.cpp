#include "client_sender.h"

#include <iostream>

ClientSender::ClientSender(Protocol& protocol, Queue<ClientToServerCmd_Client*>& send_queue):
        protocol(protocol), send_queue(send_queue) {}

void ClientSender::run() {
    try {
        while (should_keep_running()) {
            const auto* cmd = send_queue.pop();
            if (cmd) {
                auto data = cmd->to_bytes();
                protocol.send_message(data);
            }
        }
    } catch (const ClosedQueue& e) {
        return;
    } catch (const std::exception& e) {
        std::cerr << "ThreadSender: exception occurred.\n" << e.what() << std::endl;
    }
}
