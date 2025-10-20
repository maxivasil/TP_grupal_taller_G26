#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H

#include <string>

#include "../common/socket.h"

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
    Queue<int> gameloop_queue;
    ServerProtectedClients protected_clients;
    ServerGameLoop server_gameloop{gameloop_queue, protected_clients};
    Acceptor acceptor{servname, protected_clients, gameloop_queue};
};

#endif
