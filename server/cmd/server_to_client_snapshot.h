#ifndef SERVER_TO_CLIENT_SNAPSHOT_H
#define SERVER_TO_CLIENT_SNAPSHOT_H

#include <vector>

#include "../../common/buffer_utils.h"
#include "../../common/constants.h"

#include "cmd_base_server.h"

struct CarSnapshot {
    uint8_t id;
    float pos_x;
    float pos_y;
    bool collision;
    float health;
    float speed;
    uint8_t direction;
};

class ServerToClientSnapshot: public ServerToClientCmd_Server {
private:
    std::vector<CarSnapshot> cars;

public:
    explicit ServerToClientSnapshot(const std::vector<CarSnapshot>& cars);

    std::vector<uint8_t> to_bytes() const override;

    ServerToClientCmd_Server* clone() const override;
};

#endif  // SERVER_TO_CLIENT_SNAPSHOT_H
