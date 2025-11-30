#include "client_to_server_applyUpgrades_server.h"

#include <cstring>
#include <iostream>
#include <stdexcept>

#include "../../common/buffer_utils.h"
#include "../Lobby.h"
#include "../game_logic/Player.h"
#include "../game_logic/Race.h"

ClientToServerApplyUpgrades_Server::ClientToServerApplyUpgrades_Server(uint32_t client_id,
                                                                       const CarUpgrades& upgrades):
        ClientToServerCmd_Server(client_id), upgrades(upgrades) {}

void ClientToServerApplyUpgrades_Server::execute(ServerContext& ctx) {
    // Validar upgrades
    if (!ctx.players || !upgrades.isValid()) {
        return;
    }
    // Encontrar el jugador correspondiente al cliente
    auto it = std::find_if(ctx.players->begin(), ctx.players->end(),
                           [this](const auto& player) { return player->getId() == client_id; });

    if (it != ctx.players->end()) {
        (*it)->applyCarUpgrades(upgrades);
    }
}

ClientToServerApplyUpgrades_Server* ClientToServerApplyUpgrades_Server::from_bytes(
        const std::vector<uint8_t>& data, const uint32_t client_id) {
    // Estructura esperada: [command_type (1 byte) + 4 floats = 17 bytes mínimo]
    if (data.size() < 17) {
        throw std::runtime_error("ApplyUpgradesCmd: datos insuficientes (esperados 17 bytes)");
    }
    CarUpgrades upgrades;
    size_t offset = 1;
    // Deserializar los 4 floats (cada uno es 4 bytes)
    BufferUtils::read_float(data, offset, upgrades.acceleration_boost);
    BufferUtils::read_float(data, offset, upgrades.speed_boost);
    BufferUtils::read_float(data, offset, upgrades.handling_boost);
    BufferUtils::read_float(data, offset, upgrades.health_boost);
    return new ClientToServerApplyUpgrades_Server(client_id, upgrades);
}

const CarUpgrades ClientToServerApplyUpgrades_Server::get_only_for_test_upgrades() const {
    return upgrades;
}
