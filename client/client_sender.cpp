#include "client_sender.h"

#include <iostream>

ClientSender::ClientSender(Protocol& protocol, Queue<int>& send_queue):
        protocol(protocol), send_queue(send_queue) {}

void ClientSender::run() {
    try {
        while (should_keep_running()) {
            send_queue.pop();
            protocol.send_activate_nitro();
        }
    } catch (const ClosedQueue& e) {
        return;
    } catch (const std::exception& e) {
        std::cerr << "ThreadSender: exception occurred.\n" << e.what() << std::endl;
    }
}
