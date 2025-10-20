#ifndef SERVER_CLIENT_HANDLER_H
#define SERVER_CLIENT_HANDLER_H

#include <vector>

#include "../common/protocol.h"
#include "../common/queue.h"
#include "../common/socket.h"
#include "../common/thread.h"

#include "server_receiver.h"
#include "server_sender.h"

class ServerClientHandler: public Thread {
public:
    explicit ServerClientHandler(int client_id, Socket&& s, Queue<int>& gameloop_queue);
    void run() override;
    bool is_dead() const;
    void stop() override;
    void send_message(int cars_with_nitro, int msg);

private:
    int client_id;
    Protocol protocol;

    Queue<std::vector<int>> send_queue;
    ThreadReceiver receiver;
    ThreadSender sender;
};

#endif
