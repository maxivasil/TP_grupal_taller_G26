#ifndef CLIENT_TO_SERVER_CHEAT_H
#define CLIENT_TO_SERVER_CHEAT_H

#include <vector>

#include "../common/constants.h"

#include "cmd_base_server.h"

class ClientToServerCheat: public ClientToServerCmd_Server {
public:
    explicit ClientToServerCheat(uint8_t cheat_type, int client_id = -1);

    ~ClientToServerCheat() override = default;

    void execute(ServerContext& ctx) override;

    static ClientToServerCheat* from_bytes(const std::vector<uint8_t>& data, const int client_id);

private:
    uint8_t cheat_type;
};

#endif
