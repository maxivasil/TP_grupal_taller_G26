#include "session.h"

ServerSession::ServerSession(const char* servname):
        servname(servname),
        lobbiesMonitor(),
        protected_clients(),
        acceptor(servname, lobbiesMonitor, protected_clients) {}

void ServerSession::run() {
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
    lobbiesMonitor.closeAllLobbies();
    protected_clients.stop_all_and_delete();
}
