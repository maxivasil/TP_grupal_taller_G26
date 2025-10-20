#include "server_receiver.h"

#include <iostream>

ThreadReceiver::ThreadReceiver(int id, Protocol& protocol, Queue<int>& receive_queue):
        client_id(id), protocol(protocol), receive_queue(receive_queue) {}

void ThreadReceiver::run() {
    try {
        while (should_keep_running()) {
            int code = protocol.receive_activate_nitro();
            if (code <= 0)
                break;
            receive_queue.push(client_id);
        }
    } catch (const std::exception& e) {
        if (!should_keep_running() || protocol.is_connection_closed()) {
            return;
        }
        std::cerr << "ThreadReceiver: Exiting run due to exception or queue closed. " << e.what()
                  << std::endl;
    }
}
