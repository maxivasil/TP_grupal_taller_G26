#include "session.h"

ServerSession::ServerSession(const char* servname):
        servname(servname),
        gameloop_queue(),
        protected_clients(),
        server_gameloop(gameloop_queue, protected_clients),
        acceptor(servname, protected_clients, gameloop_queue) {}

void ServerSession::run() {
    server_gameloop.start();
    acceptor.start();
    while (true) {
        std::string cmd;
        if (!std::getline(std::cin, cmd))
            break;
        if (cmd == "q") {
            break;
        }
    }
    stop();
}

void ServerSession::stop() {
    acceptor.stop();
    acceptor.join();
    server_gameloop.stop();
    server_gameloop.join();
    protected_clients.stop_all_and_delete();
}
