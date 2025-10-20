#ifndef CLIENT_RECEIVER_H
#define CLIENT_RECEIVER_H

#include "../common/protocol.h"
#include "../common/queue.h"
#include "../common/thread.h"

class ClientReceiver: public Thread {
public:
    ClientReceiver(Protocol& protocol, Queue<int>& receive_queue);

    void run() override;

private:
    Protocol& protocol;
    Queue<int>& receive_queue;
};

#endif
