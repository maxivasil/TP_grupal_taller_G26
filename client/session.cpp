#include "session.h"
#include "parser.h"
#include <iostream>
#include <utility>
#include "../common/queue.h"
#include "../common/thread.h"


class ConsoleReader : public Thread {
    ClientParser& parser;
    Queue<std::pair<cmd, int>>& cmd_queue;

public:
    ConsoleReader(ClientParser& p, Queue<std::pair<cmd, int>>& q)
        : parser(p), cmd_queue(q) {}

    void run() override {
        try {
            while (should_keep_running()) {
                int lines_to_read = 0;
                cmd command = parser.parse_and_filter_line(lines_to_read); 
                cmd_queue.push({command, lines_to_read});
                if (command == EXIT) break;
            }
        } catch (const ClosedQueue&) {
        } catch (const std::exception& e) {
            std::cerr << "ConsoleReader: " << e.what() << std::endl;
        }
    }
};
int ClientSession::run() {
    try {
        receiver.start(); sender.start();
        Queue<std::pair<cmd,int>> q; ConsoleReader cr(parser, q); cr.start();
        int pending = 0; bool exit_req = false;
        while (!exit_req && receiver.is_alive() && sender.is_alive() && !protocol.is_connection_closed()) {
            if (pending > 0) {
                int code;
                if (receive_queue.try_pop(code)) {
                    if (code == INFORM_NITRO_ACTIVATED) { std::cout << "A car hit the nitro!" << std::endl; --pending; }
                    else if (code == INFORM_NITRO_EXPIRED) { std::cout << "A car is out of juice." << std::endl; --pending; }
                    continue;
                }
            }
            std::pair<cmd,int> e;
            if (q.try_pop(e)) {
                if (e.first == EXIT) exit_req = true;
                else if (e.first == NITRO) send_queue.try_push(0);
                else if (e.first == READ) pending += e.second;
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
        stop(); protocol.close_connection(); cr.stop(); q.close();
        if (receiver.is_alive()) receiver.join(); if (sender.is_alive()) sender.join(); if (cr.is_alive()) cr.join();
        return 0;
    } catch (const std::exception& e) {
        auto join_if = [&](auto& t){ if (t.is_alive()) t.join(); };
        if (!receiver.is_alive() || !sender.is_alive() || protocol.is_connection_closed()) { join_if(receiver); join_if(sender); if (!protocol.is_connection_closed()) protocol.close_connection(); return 0; }
        std::cerr << "Unexpected exception in ClientHandler" << e.what() << std::endl; return 1;
    }
}

void ClientSession::stop() {
    receive_queue.close();
    receiver.stop();
    send_queue.close();
    sender.stop();
}
