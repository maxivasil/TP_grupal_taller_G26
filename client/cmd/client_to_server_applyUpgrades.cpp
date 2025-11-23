#include "client_to_server_applyUpgrades.h"

#include <cstring>

ClientToServerApplyUpgrades::ClientToServerApplyUpgrades(const CarUpgrades& upgrades):
        upgrades(upgrades) {}

std::vector<uint8_t> ClientToServerApplyUpgrades::to_bytes() const {
    std::vector<uint8_t> data;

    uint8_t header = APPLY_UPGRADES_COMMAND;
    data.push_back(header);

    // Serializar cada boost como float (4 bytes)
    uint8_t acceleration_bytes[sizeof(float)];
    std::memcpy(acceleration_bytes, &upgrades.acceleration_boost, sizeof(float));
    data.insert(data.end(), acceleration_bytes, acceleration_bytes + sizeof(float));

    uint8_t speed_bytes[sizeof(float)];
    std::memcpy(speed_bytes, &upgrades.speed_boost, sizeof(float));
    data.insert(data.end(), speed_bytes, speed_bytes + sizeof(float));

    uint8_t handling_bytes[sizeof(float)];
    std::memcpy(handling_bytes, &upgrades.handling_boost, sizeof(float));
    data.insert(data.end(), handling_bytes, handling_bytes + sizeof(float));

    uint8_t health_bytes[sizeof(float)];
    std::memcpy(health_bytes, &upgrades.health_boost, sizeof(float));
    data.insert(data.end(), health_bytes, health_bytes + sizeof(float));

    return data;
}
