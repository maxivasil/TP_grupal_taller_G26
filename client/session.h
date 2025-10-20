#ifndef CLIENT_SESSION_H
#define CLIENT_SESSION_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "../common/constants.h"
#include "../common/protocol.h"
#include "../common/queue.h"
#include "../common/socket.h"

#include "client_receiver.h"
#include "client_sender.h"
#include "parser.h"

class ClientSession {
public:
    explicit ClientSession(const char* hostname, const char* servname, Queue<int>& recv_queue);
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
    Queue<int> send_queue;
    Queue<int>& receive_queue;
    ClientSender sender;
    ClientReceiver receiver;
};

#endif
