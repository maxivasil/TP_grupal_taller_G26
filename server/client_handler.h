#ifndef SERVER_CLIENT_HANDLER_H
#define SERVER_CLIENT_HANDLER_H

#include <vector>

#include "../common/protocol.h"
#include "../common/queue.h"
#include "../common/socket.h"
#include "../common/thread.h"
#include "cmd/cmd_server_register.h"

#include "server_receiver.h"
#include "server_sender.h"

class ServerClientHandler: public Thread {
public:
    explicit ServerClientHandler(int client_id, Socket&& s,
                                 Queue<ClientToServerCmd_Server*>& gameloop_queue);
    void run() override;
    bool is_dead() const;
    void stop() override;
    void send_message(ServerToClientCmd_Server* cmd);

private:
    int client_id;
    Protocol protocol;

    Queue<ServerToClientCmd_Server*> send_queue;
    ServerRegisteredCommands registered_commands;
    ThreadReceiver receiver;
    ThreadSender sender;
};

#endif
