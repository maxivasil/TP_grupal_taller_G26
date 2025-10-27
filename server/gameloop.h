#ifndef SERVER_GAMELOOP_H
#define SERVER_GAMELOOP_H

#include <map>
#include <vector>

#include "../common/queue.h"
#include "../common/socket.h"
#include "../common/thread.h"
#include "cmd/server_to_client_snapshot.h"

#include "protected_clients.h"

class ServerGameLoop: public Thread {
public:
    explicit ServerGameLoop(Queue<ClientToServerCmd_Server*>& gameloop_queue,
                            ServerProtectedClients& protected_clients);
    void run() override;

private:
    Queue<ClientToServerCmd_Server*>& gameloop_queue;
    ServerProtectedClients& protected_clients;
    void process_pending_commands();
    void update_game_state();
};

#endif
