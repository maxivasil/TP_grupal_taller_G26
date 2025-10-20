#ifndef THREAD_RECEIVER_H
#define THREAD_RECEIVER_H

#include "../common/protocol.h"
#include "../common/queue.h"
#include "../common/thread.h"

class ThreadReceiver: public Thread {
public:
    ThreadReceiver(int id, Protocol& protocol, Queue<int>& receive_queue);

    void run() override;

private:
    int client_id;
    Protocol& protocol;
    Queue<int>& receive_queue;
};

#endif
