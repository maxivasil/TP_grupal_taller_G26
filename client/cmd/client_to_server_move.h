#ifndef CLIENT_TO_SERVER_MOVE_H
#define CLIENT_TO_SERVER_MOVE_H

#include <cstring>
#include <vector>

#include "../../common/buffer_utils.h"
#include "../../common/constants.h"

#include "cmd_base_client.h"

#define MOVE_UP 0x01
#define MOVE_DOWN 0x02
#define MOVE_LEFT 0x03
#define MOVE_RIGHT 0x04

class ClientToServerMove: public ClientToServerCmd_Client {
public:
    explicit ClientToServerMove(uint8_t direction);
    std::vector<uint8_t> to_bytes() const override;  // Implementación de serialización
private:
    uint8_t direction;
};

#endif  // CLIENT_TO_SERVER_MOVE_H
