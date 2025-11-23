#include "server_to_client_snapshot.h"

#include <cstring>
#include <stdexcept>

#include <arpa/inet.h>

#include "../client_game.h"

ServerToClientSnapshot::ServerToClientSnapshot(std::vector<CarSnapshot> cars):
        cars_snapshot(std::move(cars)) {}

void ServerToClientSnapshot::execute(ClientContext& ctx) {
    ctx.game->update_snapshots(cars_snapshot);
}

ServerToClientSnapshot ServerToClientSnapshot::from_bytes(const std::vector<uint8_t>& data) {
    if (data.empty()) {
        throw std::runtime_error("Snapshot data is empty");
    }

    size_t offset = 0;

    uint8_t header = data[offset++];
    if (header != SNAPSHOT_COMMAND)
        throw std::runtime_error("Invalid header for snapshot");

    uint8_t car_count;
    BufferUtils::read_uint8(data, offset, car_count);

    std::vector<CarSnapshot> cars;
    cars.reserve(car_count);

    for (uint8_t i = 0; i < car_count; i++) {
        CarSnapshot car;

        if (offset + sizeof(car.id) + sizeof(car.pos_x) + sizeof(car.pos_y) + sizeof(uint8_t) +
                    sizeof(car.health) + sizeof(car.speed) + sizeof(car.angle) +
                    sizeof(car.onBridge) + sizeof(car.car_type) >
            data.size())
            throw std::runtime_error("Incomplete snapshot: missing car data");

        BufferUtils::read_uint32(data, offset, car.id);

        BufferUtils::read_float(data, offset, car.pos_x);

        BufferUtils::read_float(data, offset, car.pos_y);

        std::memcpy(&car.collision, &data[offset], sizeof(car.collision));
        offset += sizeof(car.collision);

        BufferUtils::read_float(data, offset, car.health);

        BufferUtils::read_float(data, offset, car.speed);

        BufferUtils::read_float(data, offset, car.angle);

        std::memcpy(&car.onBridge, &data[offset], sizeof(car.onBridge));
        offset += sizeof(car.onBridge);

        std::memcpy(&car.car_type, &data[offset], sizeof(car.car_type));
        offset += sizeof(car.car_type);

        std::memcpy(&car.hasInfiniteHealth, &data[offset], sizeof(car.hasInfiniteHealth));
        offset += sizeof(car.hasInfiniteHealth);

        cars.push_back(car);
    }

    return ServerToClientSnapshot(cars);
}
