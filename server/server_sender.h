#ifndef THREAD_SENDER_H
#define THREAD_SENDER_H

#include <cstdint>
#include <memory>
#include <vector>

#include "../common/protocol.h"
#include "../common/queue.h"
#include "../common/thread.h"
#include "cmd/cmd_base_server.h"

class ThreadSender: public Thread {
public:
    ThreadSender(Protocol& protocol, Queue<std::shared_ptr<ServerToClientCmd_Server>>& send_queue);

    void run() override;

private:
    Protocol& protocol;
    Queue<std::shared_ptr<ServerToClientCmd_Server>>& send_queue;
};

#endif
