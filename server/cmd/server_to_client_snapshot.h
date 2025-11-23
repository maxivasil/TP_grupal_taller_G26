#ifndef SERVER_TO_CLIENT_SNAPSHOT_H
#define SERVER_TO_CLIENT_SNAPSHOT_H

#include <vector>

#include "../../common/buffer_utils.h"
#include "../../common/constants.h"

#include "cmd_base_server.h"

struct CarSnapshot {
    int id;
    float pos_x;
    float pos_y;
    bool collision;
    float health;
    float speed;
    float angle;
    bool onBridge;
    uint8_t car_type;  // Tipo de auto: 0=Van, 1=Ferrari, 2=Celeste, 3=Jeep, 4=Pickup, 5=Limo,
                       // 6=Descapotable
    bool hasInfiniteHealth = false;  // Indica si el cheat de vida infinita está activado
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
