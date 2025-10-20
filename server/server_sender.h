#ifndef THREAD_SENDER_H
#define THREAD_SENDER_H

#include <cstdint>
#include <vector>

#include "../common/protocol.h"
#include "../common/queue.h"
#include "../common/thread.h"

class ThreadSender: public Thread {
public:
    ThreadSender(Protocol& protocol, Queue<std::vector<int>>& send_queue);

    void run() override;

private:
    Protocol& protocol;
    Queue<std::vector<int>>& send_queue;
};

#endif
