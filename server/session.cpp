#include "session.h"

ServerSession::ServerSession(const char* servname):
        servname(servname), lobbiesMonitor(), acceptor(servname, lobbiesMonitor) {}

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
}
