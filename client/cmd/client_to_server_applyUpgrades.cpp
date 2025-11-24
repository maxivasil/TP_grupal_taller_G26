#include "client_to_server_applyUpgrades.h"

#include <cstring>

#include "../../common/buffer_utils.h"

ClientToServerApplyUpgrades::ClientToServerApplyUpgrades(const CarUpgrades& upgrades):
        upgrades(upgrades) {}

std::vector<uint8_t> ClientToServerApplyUpgrades::to_bytes() const {
    std::vector<uint8_t> data;

    BufferUtils::append_uint8(data, APPLY_UPGRADES_COMMAND);

    BufferUtils::append_float(data, upgrades.acceleration_boost);

    BufferUtils::append_float(data, upgrades.speed_boost);

    BufferUtils::append_float(data, upgrades.handling_boost);

    BufferUtils::append_float(data, upgrades.health_boost);

    return data;
}
