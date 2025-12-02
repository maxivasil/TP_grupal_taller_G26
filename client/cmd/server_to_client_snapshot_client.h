#ifndef SERVER_TO_CLIENT_SNAPSHOT_CLIENT_H
#define SERVER_TO_CLIENT_SNAPSHOT_CLIENT_H

#include <string>
#include <utility>
#include <vector>

#include "../../common/constants.h"

#include "cmd_base_client.h"

struct CarSnapshot {
    uint32_t id;
    float pos_x;
    float pos_y;
    uint8_t collision;
    float health;
    float speed;
    float angle;
    uint8_t onBridge;
    uint8_t car_type;  // Tipo de auto: 0=Van, 1=Ferrari, 2=Celeste, 3=Jeep, 4=Pickup, 5=Limo,
                       // 6=Descapotable
    uint8_t hasInfiniteHealth = false;  // Indica si el cheat de vida infinita está activado
    uint8_t isNPC = false;              // True si es un auto de tráfico (NPC), false si es jugador
    std::string playerName = "";        // Nombre del jugador (vacío para NPCs)
};

class ServerToClientSnapshot_Client: public ServerToClientCmd_Client {
private:
    std::vector<CarSnapshot> cars_snapshot;
    float elapsedTime;

public:
    ServerToClientSnapshot_Client(std::vector<CarSnapshot> cars, float elapsedTime);

    virtual void execute(ClientContext& ctx) override;

    static ServerToClientSnapshot_Client from_bytes(const std::vector<uint8_t>& data);

    const std::vector<CarSnapshot>& get_only_for_test_cars_snapshot() const;
};

#endif  // SERVER_TO_CLIENT_SNAPSHOT_CLIENT_H
