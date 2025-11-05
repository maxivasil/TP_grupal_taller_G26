#ifndef SERVER_ACCEPTOR_H
#define SERVER_ACCEPTOR_H

#include <list>
#include <utility>

#include "../common/socket.h"
#include "../common/thread.h"

#include "client_handler.h"
#include "protected_clients.h"

class Acceptor: public Thread {
public:
    Acceptor(const char* servname, LobbiesMonitor& lobbiesMonitor);
    void run() override;
    void stop() override;

private:
    Socket skt;
    LobbiesMonitor& lobbiesMonitor;
    int next_client_id = {0};
};

#endif
