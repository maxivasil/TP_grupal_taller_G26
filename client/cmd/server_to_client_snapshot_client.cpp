#include "server_to_client_snapshot_client.h"

#include <cstring>
#include <stdexcept>
#include <string>

#include "../client_game.h"

ServerToClientSnapshot_Client::ServerToClientSnapshot_Client(std::vector<CarSnapshot> cars,
                                                             float elapsedTime):
        cars_snapshot(std::move(cars)), elapsedTime(elapsedTime) {}

void ServerToClientSnapshot_Client::execute(ClientContext& ctx) {
    if (!ctx.game)
        return;
    ctx.game->update_snapshots(cars_snapshot, elapsedTime);
}

ServerToClientSnapshot_Client ServerToClientSnapshot_Client::from_bytes(
        const std::vector<uint8_t>& data) {
    if (data.empty()) {
        throw std::runtime_error("Snapshot data is empty");
    }

    size_t offset = 0;

    uint8_t header = data[offset++];
    if (header != SNAPSHOT_COMMAND)
        throw std::runtime_error("Invalid header for snapshot");

    float elapsedTime;
    BufferUtils::read_float(data, offset, elapsedTime);

    uint8_t car_count;
    BufferUtils::read_uint8(data, offset, car_count);

    std::vector<CarSnapshot> cars;
    cars.reserve(car_count);

    for (uint8_t i = 0; i < car_count; i++) {
        CarSnapshot car;

        if (offset + sizeof(car.id) + sizeof(car.pos_x) + sizeof(car.pos_y) +
                    sizeof(car.collision) + sizeof(car.health) + sizeof(car.speed) +
                    sizeof(car.angle) + sizeof(car.onBridge) + sizeof(car.car_type) +
                    sizeof(car.hasInfiniteHealth) + sizeof(car.isNPC) >
            data.size())
            throw std::runtime_error("Incomplete snapshot: missing car data");

        BufferUtils::read_uint32(data, offset, car.id);

        BufferUtils::read_float(data, offset, car.pos_x);

        BufferUtils::read_float(data, offset, car.pos_y);

        BufferUtils::read_uint8(data, offset, car.collision);

        BufferUtils::read_float(data, offset, car.health);

        BufferUtils::read_float(data, offset, car.speed);

        BufferUtils::read_float(data, offset, car.angle);

        BufferUtils::read_uint8(data, offset, car.onBridge);

        BufferUtils::read_uint8(data, offset, car.car_type);

        BufferUtils::read_uint8(data, offset, car.hasInfiniteHealth);

        BufferUtils::read_uint8(data, offset, car.isNPC);

        // Leer nombre del jugador
        if (offset + sizeof(uint16_t) <= data.size()) {
            uint16_t nameLen;
            BufferUtils::read_uint16(data, offset, nameLen);
            if (nameLen > 0 && offset + nameLen <= data.size()) {
                car.playerName =
                        std::string(reinterpret_cast<const char*>(data.data() + offset), nameLen);
                offset += nameLen;
            }
        }

        cars.push_back(car);
    }

    return ServerToClientSnapshot_Client(cars, elapsedTime);
}

const std::vector<CarSnapshot>& ServerToClientSnapshot_Client::get_only_for_test_cars_snapshot()
        const {
    return cars_snapshot;
}
