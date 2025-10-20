#include "acceptor.h"

#define SHUTDOWN 2

Acceptor::Acceptor(const char* servname, ServerProtectedClients& protected_clients,
                   Queue<int>& gameloop_queue):
        skt(servname), protected_clients(protected_clients), gameloop_queue(gameloop_queue) {}

void Acceptor::run() {
    try {
        while (should_keep_running()) {
            Socket peer = skt.accept();
            auto* c = new ServerClientHandler(next_client_id++, std::move(peer), gameloop_queue);
            protected_clients.add_client(c);
            c->start();
            reap();
        }
    } catch (...) {
        if (!should_keep_running()) {
            return;
        }
    }
}

void Acceptor::reap() { protected_clients.stop_and_delete_dead(); }

void Acceptor::stop() {
    Thread::stop();
    skt.shutdown(SHUTDOWN);
    skt.close();
}
