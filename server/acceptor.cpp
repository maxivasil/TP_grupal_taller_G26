#include "acceptor.h"

#include <memory>

#define SHUTDOWN 2

Acceptor::Acceptor(const char* servname, LobbiesMonitor& lobbiesMonitor,
                   ServerProtectedClients& protected_clients):
        skt(servname), lobbiesMonitor(lobbiesMonitor), protected_clients(protected_clients) {}

void Acceptor::run() {
    try {
        while (should_keep_running()) {
            Socket peer = skt.accept();
            auto c = std::make_shared<ServerClientHandler>(next_client_id++, std::move(peer),
                                                           lobbiesMonitor);
            protected_clients.add_client(c);
            c->start();
            reap();
        }
    } catch (...) {
        if (!should_keep_running()) {
            stop();
            return;
        }
    }
}

void Acceptor::reap() { protected_clients.stop_and_delete_dead(); }

void Acceptor::stop() {
    Thread::stop();
    if (!skt.is_stream_recv_closed() || !skt.is_stream_send_closed()) {
        skt.shutdown(SHUTDOWN);
        skt.close();
    }
}
