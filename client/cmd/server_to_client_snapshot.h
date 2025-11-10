#ifndef SERVER_TO_CLIENT_SNAPSHOT_H
#define SERVER_TO_CLIENT_SNAPSHOT_H

#include <utility>
#include <vector>

#include "../../common/constants.h"

#include "cmd_base_client.h"

struct CarSnapshot {
    uint8_t id;
    float pos_x;
    float pos_y;
    bool collision;
    float health;
    float speed;
    float angle;
    bool onBridge;
};

class ServerToClientSnapshot: public ServerToClientCmd_Client {
private:
    std::vector<CarSnapshot> cars_snapshot;

public:
    explicit ServerToClientSnapshot(std::vector<CarSnapshot> cars);

    virtual void execute(Game& game) override;

    static ServerToClientSnapshot from_bytes(const std::vector<uint8_t>& data);
};

#endif  // SERVER_TO_CLIENT_SNAPSHOT_H
