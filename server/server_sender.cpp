#include "server_sender.h"

#include <iostream>

ThreadSender::ThreadSender(Protocol& protocol,
                           Queue<std::shared_ptr<ServerToClientCmd_Server>>& send_queue):
        protocol(protocol), send_queue(send_queue) {}

void ThreadSender::run() {
    try {
        while (should_keep_running()) {
            std::shared_ptr<ServerToClientCmd_Server> raw = send_queue.pop();
            if (raw) {
                auto data = raw->to_bytes();
                protocol.send_message(data);
            }
        }
    } catch (const ClosedQueue& e) {
        return;
    } catch (const std::exception& e) {
        std::cerr << "ThreadSender: exception occurred.\n" << e.what() << std::endl;
    }
}
