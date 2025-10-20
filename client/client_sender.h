#ifndef CLIENT_SENDER_H
#define CLIENT_SENDER_H

#include <cstdint>
#include <vector>

#include "../common/protocol.h"
#include "../common/queue.h"
#include "../common/thread.h"

class ClientSender: public Thread {
public:
    ClientSender(Protocol& protocol, Queue<int>& send_queue);

    void run() override;

private:
    Protocol& protocol;
    Queue<int>& send_queue;
};

#endif
