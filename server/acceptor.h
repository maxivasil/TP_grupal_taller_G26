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
    explicit Acceptor(const char* servname, ServerProtectedClients& protected_clients,
                      Queue<ClientToServerCmd_Server*>& gameloop_queue);
    void run() override;
    void stop() override;

private:
    Socket skt;
    ServerProtectedClients& protected_clients;
    Queue<ClientToServerCmd_Server*>& gameloop_queue;
    void reap();
    int next_client_id = {0};
};

#endif
