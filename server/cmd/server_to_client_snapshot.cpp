#include "server_to_client_snapshot.h"

/**
 * <HEADER>;<CANT_AUTOS>;<ID_AUTO>;<POS_X>;<POS_Y>;<COLISION¿?>;<HEALTH>;<SPEED>;<ANGLE>;<ON_BRIDGE¿?>;<CAR_TYPE>;<HAS_INFINITE_HEALTH¿?>;<IS_NPC¿?>
 */
ServerToClientSnapshot::ServerToClientSnapshot(const std::vector<CarSnapshot>& cars): cars(cars) {}

std::vector<uint8_t> ServerToClientSnapshot::to_bytes() const {
    std::vector<uint8_t> data;

    uint8_t header = SNAPSHOT_COMMAND;
    data.push_back(header);

    uint8_t car_count = static_cast<uint8_t>(cars.size());
    BufferUtils::append_bytes(data, &car_count, sizeof(car_count));

    for (const auto& car: cars) {
        BufferUtils::append_bytes(data, &car.id, sizeof(car.id));
        BufferUtils::append_bytes(data, &car.pos_x, sizeof(car.pos_x));
        BufferUtils::append_bytes(data, &car.pos_y, sizeof(car.pos_y));

        uint8_t collision = car.collision ? 1 : 0;
        BufferUtils::append_bytes(data, &collision, sizeof(collision));

        BufferUtils::append_bytes(data, &car.health, sizeof(car.health));
        BufferUtils::append_bytes(data, &car.speed, sizeof(car.speed));
        BufferUtils::append_bytes(data, &car.angle, sizeof(car.angle));
        uint8_t onBridge = car.onBridge ? 1 : 0;
        BufferUtils::append_bytes(data, &onBridge, sizeof(onBridge));
        BufferUtils::append_bytes(data, &car.car_type, sizeof(car.car_type));
        uint8_t hasInfiniteHealth = car.hasInfiniteHealth ? 1 : 0;
        BufferUtils::append_bytes(data, &hasInfiniteHealth, sizeof(hasInfiniteHealth));
        uint8_t isNPC = car.isNPC ? 1 : 0;
        BufferUtils::append_bytes(data, &isNPC, sizeof(isNPC));
    }

    return data;
}

ServerToClientCmd_Server* ServerToClientSnapshot::clone() const {
    return new ServerToClientSnapshot(*this);
}
