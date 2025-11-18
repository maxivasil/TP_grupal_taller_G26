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
    uint8_t car_type;  // Tipo de auto: 0=Van, 1=Ferrari, 2=Celeste, 3=Jeep, 4=Pickup, 5=Limo, 6=Descapotable
    bool hasInfiniteHealth = false;  // Indica si el cheat de vida infinita está activado
    bool isNPC = false;  // True si es un auto de tráfico (NPC), false si es jugador
};

class ServerToClientSnapshot: public ServerToClientCmd_Client {
private:
    std::vector<CarSnapshot> cars_snapshot;

public:
    explicit ServerToClientSnapshot(std::vector<CarSnapshot> cars);

    virtual void execute(ClientContext& ctx) override;

    static ServerToClientSnapshot from_bytes(const std::vector<uint8_t>& data);
};

#endif  // SERVER_TO_CLIENT_SNAPSHOT_H
