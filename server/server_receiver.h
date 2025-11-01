#ifndef THREAD_RECEIVER_H
#define THREAD_RECEIVER_H

#include <unordered_map>
#include <utility>
#include <vector>

#include "../common/protocol.h"
#include "../common/queue.h"
#include "../common/thread.h"
#include "cmd/cmd_base_server.h"

class ThreadReceiver: public Thread {
public:
    ThreadReceiver(
            int id, Protocol& protocol, Queue<ClientToServerCmd_Server*>& receive_queue,
            const std::unordered_map<
                    uint8_t, std::function<ClientToServerCmd_Server*(const std::vector<uint8_t>&)>>&
                    registry);

    void run() override;

private:
    int client_id;
    Protocol& protocol;
    Queue<ClientToServerCmd_Server*>& receive_queue;
    const std::unordered_map<uint8_t,
                             std::function<ClientToServerCmd_Server*(const std::vector<uint8_t>&)>>&
            registry;
};

#endif
