#include "client_receiver.h"

#include <iostream>

ClientReceiver::ClientReceiver(Protocol& protocol, Queue<int>& receive_queue):
        protocol(protocol), receive_queue(receive_queue) {}

void ClientReceiver::run() {
    try {
        while (should_keep_running()) {
            int code = protocol.receive_cars_with_nitro();
            if (code <= 0)
                break;
            receive_queue.push(code);
        }
    } catch (const std::exception& e) {
        if (!should_keep_running() || protocol.is_connection_closed()) {
            return;
        }
        std::cerr << "ThreadReceiver: Exiting run due to exception or queue closed. " << e.what()
                  << std::endl;
    }
}
