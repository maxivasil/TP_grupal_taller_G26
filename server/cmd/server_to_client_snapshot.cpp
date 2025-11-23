#include "server_to_client_snapshot.h"

/**
 * <HEADER>;<CANT_AUTOS>;<ID_AUTO>;<POS_X>;<POS_Y>;<COLISION¿?>;<HEALTH>;<SPEED>;<ANGLE>;<ON_BRIDGE¿?>;<CAR_TYPE>;<HAS_INFINITE_HEALTH¿?>
 */
ServerToClientSnapshot::ServerToClientSnapshot(const std::vector<CarSnapshot>& cars): cars(cars) {}

std::vector<uint8_t> ServerToClientSnapshot::to_bytes() const {
    std::vector<uint8_t> data;

    BufferUtils::append_uint8(data, SNAPSHOT_COMMAND);

    BufferUtils::append_uint8(data, static_cast<uint8_t>(cars.size()));

    for (const auto& car: cars) {
        BufferUtils::append_uint32(data, car.id);
        BufferUtils::append_float(data, car.pos_x);
        BufferUtils::append_float(data, car.pos_y);

        uint8_t collision = car.collision ? 1 : 0;
        BufferUtils::append_bytes(data, &collision, sizeof(collision));

        BufferUtils::append_float(data, car.health);
        BufferUtils::append_float(data, car.speed);
        BufferUtils::append_float(data, car.angle);

        uint8_t onBridge = car.onBridge ? 1 : 0;
        BufferUtils::append_bytes(data, &onBridge, sizeof(onBridge));
        BufferUtils::append_bytes(data, &car.car_type, sizeof(car.car_type));
        uint8_t hasInfiniteHealth = car.hasInfiniteHealth ? 1 : 0;
        BufferUtils::append_bytes(data, &hasInfiniteHealth, sizeof(hasInfiniteHealth));
    }

    return data;
}

ServerToClientCmd_Server* ServerToClientSnapshot::clone() const {
    return new ServerToClientSnapshot(*this);
}
