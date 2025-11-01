#ifndef CLIENT_SENDER_H
#define CLIENT_SENDER_H

#include <cstdint>
#include <memory>
#include <vector>

#include "../common/protocol.h"
#include "../common/queue.h"
#include "../common/thread.h"
#include "cmd/cmd_base_client.h"

class ClientSender: public Thread {
public:
    ClientSender(Protocol& protocol, Queue<ClientToServerCmd_Client*>& send_queue);

    void run() override;

private:
    Protocol& protocol;
    Queue<ClientToServerCmd_Client*>& send_queue;
};

#endif
