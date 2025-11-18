#ifndef CLIENT_RECEIVER_H
#define CLIENT_RECEIVER_H

#include <unordered_map>
#include <utility>
#include <vector>

#include "../common/protocol.h"
#include "../common/queue.h"
#include "../common/thread.h"
#include "cmd/cmd_base_client.h"

class ClientReceiver: public Thread {
public:
    ClientReceiver(
            Protocol& protocol, Queue<ServerToClientCmd_Client*>& receive_queue,
            const std::unordered_map<
                    uint8_t, std::function<ServerToClientCmd_Client*(const std::vector<uint8_t>&)>>&
                    registry);

    void run() override;

private:
    Protocol& protocol;
    Queue<ServerToClientCmd_Client*>& receive_queue;
    const std::unordered_map<uint8_t,
                             std::function<ServerToClientCmd_Client*(const std::vector<uint8_t>&)>>&
            registry;
};

#endif
