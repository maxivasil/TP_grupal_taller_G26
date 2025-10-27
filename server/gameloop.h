#ifndef SERVER_GAMELOOP_H
#define SERVER_GAMELOOP_H

#include <map>

#include "../common/queue.h"
#include "../common/socket.h"
#include "../common/thread.h"

#include "protected_clients.h"

struct actives_nitro_info {
    std::map<int, int> nitro_timers;
    int count;
};

class ServerGameLoop: public Thread {
public:
    explicit ServerGameLoop(Queue<ClientToServerCmd_Server*>& gameloop_queue,
                            ServerProtectedClients& protected_clients);
    void run() override;

private:
    Queue<ClientToServerCmd_Server*>& gameloop_queue;
    ServerProtectedClients& protected_clients;
    actives_nitro_info actives_nitro;
    const int nitro_duration_loops = 12;
    void process_pending_commands();
    void update_game_state();
};

#endif
