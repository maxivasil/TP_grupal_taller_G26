#ifndef CLIENT_TO_SERVER_CHEAT_CLIENT_H
#define CLIENT_TO_SERVER_CHEAT_CLIENT_H

#include <cstdint>
#include <vector>

#include "../../common/constants.h"
#include "cmd_base_client.h"

class ClientToServerCheat : public ClientToServerCmd_Client {
public:
    explicit ClientToServerCheat(uint8_t cheat_type);

    ~ClientToServerCheat() override = default;

    std::vector<uint8_t> to_bytes() const override;

private:
    uint8_t cheat_type;
};

#endif
