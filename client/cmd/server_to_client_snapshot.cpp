#include "server_to_client_snapshot.h"

#include <cstring>
#include <stdexcept>

#include <arpa/inet.h>

#include "../client_game.h"

ServerToClientSnapshot::ServerToClientSnapshot(std::vector<CarSnapshot> cars):
        cars_snapshot(std::move(cars)) {}

void ServerToClientSnapshot::execute(ClientContext& ctx) {
    ctx.game->update_snapshots(cars_snapshot);

    // std::cout << "[Snapshot recibido] Autos en escena: " << cars_snapshot.size() << std::endl;

    // for (const auto& car: cars_snapshot) {
    //     std::cout << "  Auto ID: " << car.id << " | Posición: (" << car.pos_x << ", " << car.pos_y
    //               << ")"
    //               << " | Colisión: " << (car.collision ? "sí" : "no")
    //               << " | Vida: " << static_cast<int>(car.health) << " | Velocidad: " << car.speed
    //               << " | Ángulo: " << car.angle << " | OnBridge: " << (car.onBridge ? "SI" : "NO")
    //               << std::endl;
    // }
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
    std::memcpy(&car_count, &data[offset], sizeof(uint8_t));
    offset += sizeof(uint8_t);

    std::vector<CarSnapshot> cars;
    cars.reserve(car_count);

    for (uint8_t i = 0; i < car_count; i++) {
        CarSnapshot car;

        if (offset + sizeof(car.id) + sizeof(car.pos_x) + sizeof(car.pos_y) + sizeof(uint8_t) +
                    sizeof(car.health) + sizeof(car.speed) + sizeof(car.angle) +
                    sizeof(car.onBridge) >
            data.size())
            throw std::runtime_error("Incomplete snapshot: missing car data");

        std::memcpy(&car.id, &data[offset], sizeof(car.id));
        offset += sizeof(car.id);

        std::memcpy(&car.pos_x, &data[offset], sizeof(car.pos_x));
        offset += sizeof(car.pos_x);

        std::memcpy(&car.pos_y, &data[offset], sizeof(car.pos_y));
        offset += sizeof(car.pos_y);

        std::memcpy(&car.collision, &data[offset], sizeof(car.collision));
        offset += sizeof(car.collision);

        std::memcpy(&car.health, &data[offset], sizeof(car.health));
        offset += sizeof(car.health);

        std::memcpy(&car.speed, &data[offset], sizeof(car.speed));
        offset += sizeof(car.speed);

        std::memcpy(&car.angle, &data[offset], sizeof(car.angle));
        offset += sizeof(car.angle);

        std::memcpy(&car.onBridge, &data[offset], sizeof(car.onBridge));
        offset += sizeof(car.onBridge);

        cars.push_back(car);
    }

    return ServerToClientSnapshot(cars);
}
