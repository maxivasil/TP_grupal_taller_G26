#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H

#include <string>

#include "../common/socket.h"

#include "LobbiesMonitor.h"
#include "acceptor.h"
#include "gameloop.h"
#include "protected_clients.h"

class ServerSession {
public:
    explicit ServerSession(const char* servname);
    ~ServerSession() = default;

    void run();
    void stop();

private:
    const char* servname;
    LobbiesMonitor lobbiesMonitor;
    Acceptor acceptor;
};

#endif
