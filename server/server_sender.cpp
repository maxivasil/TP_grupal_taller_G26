#include "server_sender.h"

#include <iostream>

ThreadSender::ThreadSender(Protocol& protocol, Queue<ServerToClientCmd_Server*>& send_queue):
        protocol(protocol), send_queue(send_queue) {}

void ThreadSender::run() {
    try {
        while (should_keep_running()) {
            ServerToClientCmd_Server* cmd = send_queue.pop();
            if (cmd) {
                auto data = cmd->to_bytes();
                protocol.send_message(data);
                delete cmd;
            }
        }
    } catch (const ClosedQueue& e) {
        return;
    } catch (const std::exception& e) {
        std::cerr << "ThreadSender: exception occurred.\n" << e.what() << std::endl;
    }
}
