#ifndef CLIENT_TO_SERVER_MOVE_SERVER_H
#define CLIENT_TO_SERVER_MOVE_SERVER_H

#include <vector>

#include "../../common/constants.h"

#include "cmd_base_server.h"

class ClientToServerMove_Server: public ClientToServerCmd_Server {
public:
    explicit ClientToServerMove_Server(uint8_t direction, uint32_t client_id);

    void execute(ServerContext& ctx) override;

    static ClientToServerMove_Server* from_bytes(const std::vector<uint8_t>& data,
                                                 const uint32_t client_id);

    uint8_t get_only_for_test_direction() const;

private:
    uint8_t direction;
};

#endif  // CLIENT_TO_SERVER_MOVE_SERVER_H
