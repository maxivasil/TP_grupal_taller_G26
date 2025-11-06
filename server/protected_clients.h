#ifndef SERVER_PROTECTED_CLIENTS_H
#define SERVER_PROTECTED_CLIENTS_H

#include <list>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

#include "../common/queue.h"
#include "../common/socket.h"
#include "../common/thread.h"
#include "cmd/cmd_base_server.h"

class ServerClientHandler;

class ServerProtectedClients {
public:
    ServerProtectedClients();
    void broadcast(std::shared_ptr<ServerToClientCmd_Server> cmd);
    void add_client(ServerClientHandler* client);
    void stop_and_delete_dead();
    void stop_all_and_delete();
    size_t size();

private:
    std::list<ServerClientHandler*> clients;
    std::mutex m;
};

#endif
