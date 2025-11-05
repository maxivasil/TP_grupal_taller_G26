#include "acceptor.h"

#define SHUTDOWN 2

Acceptor::Acceptor(const char* servname, LobbiesMonitor& lobbiesMonitor):
        skt(servname), lobbiesMonitor(lobbiesMonitor) {}

void Acceptor::run() {
    try {
        while (should_keep_running()) {
            Socket peer = skt.accept();
            auto* c = new ServerClientHandler(next_client_id++, std::move(peer), lobbiesMonitor);
            c->start();
        }
    } catch (...) {
        if (!should_keep_running()) {
            return;
        }
    }
}

void Acceptor::stop() {
    Thread::stop();
    skt.shutdown(SHUTDOWN);
    skt.close();
}
