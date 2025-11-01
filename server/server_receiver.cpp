#include "server_receiver.h"

#include <iostream>

ThreadReceiver::ThreadReceiver(
        int id, Protocol& protocol, Queue<ClientToServerCmd_Server*>& receive_queue,
        const std::unordered_map<
                uint8_t, std::function<ClientToServerCmd_Server*(const std::vector<uint8_t>&)>>&
                registry):
        client_id(id), protocol(protocol), receive_queue(receive_queue), registry(registry) {}

void ThreadReceiver::run() {
    try {
        while (should_keep_running()) {
            std::vector<uint8_t> data = protocol.recv_full_message();
            if (data.empty())
                continue;
            auto cmd = ClientToServerCmd_Server::from_bytes(data, registry);
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
