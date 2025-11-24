#ifndef CLIENT_TO_SERVER_MOVE_H
#define CLIENT_TO_SERVER_MOVE_H

#include <vector>

#include "../../common/constants.h"

#include "cmd_base_server.h"

class ClientToServerMove: public ClientToServerCmd_Server {
public:
    explicit ClientToServerMove(uint8_t direction, uint32_t client_id);

    void execute(ServerContext& ctx) override;

    static ClientToServerMove* from_bytes(const std::vector<uint8_t>& data,
                                          const uint32_t client_id);

private:
    uint8_t direction;
};

#endif  // CLIENT_TO_SERVER_MOVE_H
