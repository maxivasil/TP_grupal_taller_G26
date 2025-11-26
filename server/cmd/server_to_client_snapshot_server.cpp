#include "server_to_client_snapshot_server.h"

/**
 * <HEADER>;<CANT_AUTOS>;<ID_AUTO>;<POS_X>;<POS_Y>;<COLISION¿?>;<HEALTH>;<SPEED>;<ANGLE>;<ON_BRIDGE¿?>;<CAR_TYPE>;<HAS_INFINITE_HEALTH¿?>;<IS_NPC¿?>
 */
ServerToClientSnapshot_Server::ServerToClientSnapshot_Server(const std::vector<CarSnapshot>& cars,
                                                             float elapsedTime):
        cars(cars), elapsedTime(elapsedTime) {}

std::vector<uint8_t> ServerToClientSnapshot_Server::to_bytes() const {
    std::vector<uint8_t> data;

    BufferUtils::append_uint8(data, SNAPSHOT_COMMAND);

    BufferUtils::append_float(data, elapsedTime);

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
        uint8_t isNPC = car.isNPC ? 1 : 0;
        BufferUtils::append_bytes(data, &isNPC, sizeof(isNPC));
    }

    return data;
}

ServerToClientCmd_Server* ServerToClientSnapshot_Server::clone() const {
    return new ServerToClientSnapshot_Server(*this);
}
