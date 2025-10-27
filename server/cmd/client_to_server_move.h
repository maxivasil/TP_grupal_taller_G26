#ifndef CLIENT_TO_SERVER_MOVE_H
#define CLIENT_TO_SERVER_MOVE_H

#include <vector>

#include "cmd_base_server.h"

#define MOVE_UP 0x01
#define MOVE_DOWN 0x02
#define MOVE_LEFT 0x03
#define MOVE_RIGHT 0x04

class ClientToServerMove: public ClientToServerCmd_Server {
public:
    explicit ClientToServerMove(uint8_t direction);

    void execute() override;

    static ClientToServerMove* from_bytes(const std::vector<uint8_t>& data);

private:
    uint8_t direction;
};

#endif  // CLIENT_TO_SERVER_MOVE_H
