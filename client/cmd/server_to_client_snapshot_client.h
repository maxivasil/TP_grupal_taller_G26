#ifndef SERVER_TO_CLIENT_SNAPSHOT_CLIENT_H
#define SERVER_TO_CLIENT_SNAPSHOT_CLIENT_H

#include <utility>
#include <vector>

#include "../../common/constants.h"

#include "cmd_base_client.h"

struct CarSnapshot {
    uint32_t id;
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

class ServerToClientSnapshot_Client: public ServerToClientCmd_Client {
private:
    std::vector<CarSnapshot> cars_snapshot;

public:
    explicit ServerToClientSnapshot_Client(std::vector<CarSnapshot> cars);

    virtual void execute(ClientContext& ctx) override;

    static ServerToClientSnapshot_Client from_bytes(const std::vector<uint8_t>& data);

    const std::vector<CarSnapshot>& get_only_for_test_cars_snapshot() const;
};

#endif  // SERVER_TO_CLIENT_SNAPSHOT_CLIENT_H
