#ifndef SERVER_CLIENT_HANDLER_H
#define SERVER_CLIENT_HANDLER_H

#include <memory>
#include <string>
#include <vector>

#include "../common/protocol.h"
#include "../common/queue.h"
#include "../common/socket.h"
#include "../common/thread.h"
#include "cmd/cmd_server_register.h"
#include "cmd/server_to_client_assign_id_server.h"

#include "server_receiver.h"
#include "server_sender.h"

class LobbiesMonitor;

class ServerClientHandler: public Thread {
public:
    explicit ServerClientHandler(uint32_t client_id, Socket&& s, LobbiesMonitor& lobbiesMonitor);
    void run() override;
    bool is_dead() const;
    void stop() override;
    void send_message(std::shared_ptr<ServerToClientCmd_Server> cmd);
    Queue<ClientToServerCmd_Server*>* createLobby(const std::string& lobbyId);
    Queue<ClientToServerCmd_Server*>* joinLobby(const std::string& lobbyId);
    void initReceiver(Queue<ClientToServerCmd_Server*>& gameloop_queue);

private:
    uint32_t client_id;
    Protocol protocol;

    Queue<std::shared_ptr<ServerToClientCmd_Server>> send_queue;
    ServerRegisteredCommands registered_commands;
    std::unique_ptr<ThreadReceiver> receiver;
    ThreadSender sender;
    LobbiesMonitor& lobbiesMonitor;
};

#endif
