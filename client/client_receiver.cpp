#include "client_receiver.h"

#include <iostream>

ClientReceiver::ClientReceiver(
        Protocol& protocol, Queue<ServerToClientCmd_Client*>& receive_queue,
        const std::unordered_map<
                uint8_t, std::function<ServerToClientCmd_Client*(const std::vector<uint8_t>&)>>&
                registry):
        protocol(protocol), receive_queue(receive_queue), registry(registry) {}

void ClientReceiver::run() {
    try {
        while (should_keep_running()) {
            std::vector<uint8_t> full_message = protocol.recv_full_message();
            if (full_message.empty())
                break;

            uint8_t header = full_message[0];
            if (header == ServerToClientCmd_Client::INVALID)
                break;

            auto cmd = ServerToClientCmd_Client::from_bytes(full_message, registry);
            if (cmd)
                receive_queue.push(std::move(cmd));
        }
    } catch (const std::exception& e) {
        if (!should_keep_running() || protocol.is_connection_closed()) {
            return;
        }
        std::cerr << "ThreadReceiver: Exiting run due to exception or queue closed. " << e.what()
                  << std::endl;
    }
}
