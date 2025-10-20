#include "session.h"

ClientSession::ClientSession(const char* hostname, const char* servname, Queue<int>& recv_queue):
        protocol(hostname, servname),
        parser(),
        send_queue(),
        receive_queue(recv_queue),
        sender(protocol, send_queue),
        receiver(protocol, receive_queue) {}

int ClientSession::run() {
    try {
        receiver.start();
        sender.start();
        while (true) {
            int lines_to_read = 0;
            cmd command = parser.parse_and_filter_line(lines_to_read);
            if (command == EXIT) {
                stop();
                break;
            } else if (command == NITRO) {
                send_queue.try_push(0);
            } else if (command == READ) {
                int i = 0;
                while (i < lines_to_read) {
                    int code;
                    if (receive_queue.try_pop(code)) {
                        if (code == INFORM_NITRO_ACTIVATED) {
                            std::cout << "A car hit the nitro!" << std::endl;
                        } else if (code == INFORM_NITRO_EXPIRED) {
                            std::cout << "A car is out of juice." << std::endl;
                        }
                        i++;
                    }
                }
            }
        }
        protocol.close_connection();
        receiver.join();
        sender.join();
        return 0;
    } catch (const std::exception& e) {
        if (!receiver.is_alive() || !sender.is_alive() || protocol.is_connection_closed()) {
            if (receiver.is_alive()) {
                receiver.join();
            }
            if (sender.is_alive()) {
                sender.join();
            }
            if (!protocol.is_connection_closed()) {
                protocol.close_connection();
            }
            return 0;
        }
        std::cerr << "Unexpected exception in ClientHandler" << e.what() << std::endl;
        return 1;
    }
}

void ClientSession::stop() {
    receive_queue.close();
    receiver.stop();
    send_queue.close();
    sender.stop();
}
