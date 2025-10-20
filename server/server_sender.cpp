#include "server_sender.h"

#include <iostream>

ThreadSender::ThreadSender(Protocol& protocol, Queue<std::vector<int>>& send_queue):
        protocol(protocol), send_queue(send_queue) {}

void ThreadSender::run() {
    try {
        while (should_keep_running()) {
            std::vector<int> msg = send_queue.pop();
            protocol.send_cars_with_nitro(msg[0], static_cast<uint8_t>(msg[1]));
        }
    } catch (const ClosedQueue& e) {
        return;
    } catch (const std::exception& e) {
        std::cerr << "ThreadSender: exception occurred.\n" << e.what() << std::endl;
    }
}
