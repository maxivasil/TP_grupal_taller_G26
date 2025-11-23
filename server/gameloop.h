#ifndef SERVER_GAMELOOP_H
#define SERVER_GAMELOOP_H

#include <map>
#include <memory>
#include <set>
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

class Player;

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

    std::vector<AccumulatedResultDTO> accumulatedResults;

    void process_pending_commands(ServerContext& ctx);
    void update_game_state(const Race& race);
    void send_partial_results(Race& race, std::set<int>& playersWhoAlreadyReceivedPartial);
    void send_acumulated_results(const Race& race,
                                 std::vector<std::unique_ptr<Player>> const& players,
                                 bool& resultsAlreadySent);
};

#endif
