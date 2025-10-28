#ifndef CLIENT_SESSION_H
#define CLIENT_SESSION_H

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../common/constants.h"
#include "../common/protocol.h"
#include "../common/queue.h"
#include "../common/socket.h"
#include "cmd/cmd_base_client.h"
#include "cmd/cmd_client_register.h"

#include "client_receiver.h"
#include "client_sender.h"
#include "parser.h"

class ClientSession {
public:
    explicit ClientSession(const char* hostname, const char* servname,
                           Queue<ServerToClientCmd_Client*>& recv_queue);
    ~ClientSession() = default;

    /**
     * Inicia la sesión del cliente, leyendo comandos desde stdin y
     * ejecutándolos hasta recibir el comando "exit".
     * @return 0 si la sesión finalizó correctamente, 1 en caso de error
     */
    int run();
    void stop();

private:
    Protocol protocol;
    ClientParser parser;
    ClientRegisteredCommands registered_commands;
    Queue<ClientToServerCmd_Client*> send_queue;
    Queue<ServerToClientCmd_Client*>& receive_queue;
    ClientSender sender;
    ClientReceiver receiver;
};

#endif
