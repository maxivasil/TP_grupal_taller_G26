#ifndef SERVER_PROTECTED_CLIENTS_H
#define SERVER_PROTECTED_CLIENTS_H

#include <list>
#include <mutex>
#include <utility>
#include <vector>

#include "../common/queue.h"
#include "../common/socket.h"
#include "../common/thread.h"

#include "client_handler.h"

class ServerProtectedClients {
public:
    ServerProtectedClients();
    void broadcast(const ServerToClientCmd_Server& cmd);
    void add_client(ServerClientHandler* client);
    void stop_and_delete_dead();
    void stop_all_and_delete();

private:
    std::list<ServerClientHandler*> clients;
    std::mutex m;
};

#endif
