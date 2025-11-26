#ifndef CLIENT_TO_SERVER_CHEAT_SERVER_H
#define CLIENT_TO_SERVER_CHEAT_SERVER_H

#include <vector>

#include "../common/constants.h"

#include "cmd_base_server.h"

class ClientToServerCheat_Server: public ClientToServerCmd_Server {
public:
    explicit ClientToServerCheat_Server(uint8_t cheat_type, uint32_t client_id);

    ~ClientToServerCheat_Server() override = default;

    void execute(ServerContext& ctx) override;

    static ClientToServerCheat_Server* from_bytes(const std::vector<uint8_t>& data,
                                                  const uint32_t client_id);

    uint8_t get_only_for_test_cheat_type() const;

private:
    uint8_t cheat_type;
};

#endif
