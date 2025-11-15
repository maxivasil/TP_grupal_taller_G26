#ifndef SERVER_GAMELOOP_H
#define SERVER_GAMELOOP_H

#include <map>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../common/queue.h"
#include "../common/socket.h"
#include "../common/thread.h"
#include "cmd/server_to_client_accumulatedResults.h"
#include "cmd/server_to_client_snapshot.h"
#include "cmd/server_to_client_startingRace.h"

#include "LobbyStatus.h"
#include "protected_clients.h"

class ServerGameLoop: public Thread {
public:
    explicit ServerGameLoop(Queue<ClientToServerCmd_Server*>& gameloop_queue,
                            ServerProtectedClients& protected_clients, LobbyStatus& status,
                            struct Lobby* lobby);
    void run() override;

private:
    Queue<ClientToServerCmd_Server*>& gameloop_queue;
    ServerProtectedClients& protected_clients;
    LobbyStatus& status;
    struct Lobby* lobby;

    std::unordered_map<int, AccumulatedInfo> accumulatedResults;

    void process_pending_commands(ServerContext& ctx);
    void update_game_state(Race& race);
};

#endif
