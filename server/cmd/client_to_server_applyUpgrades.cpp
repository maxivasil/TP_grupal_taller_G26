#include "client_to_server_applyUpgrades.h"

#include <cstring>
#include <iostream>
#include <stdexcept>

#include "../Lobby.h"
#include "../game_logic/Player.h"
#include "../game_logic/Race.h"

// Comando ID para aplicar mejoras (0x0D)
#define APPLY_UPGRADES_COMMAND 0x0D

ClientToServerApplyUpgrades::ClientToServerApplyUpgrades(int client_id,
                                                         const CarUpgrades& upgrades):
        ClientToServerCmd_Server(client_id), upgrades(upgrades) {}

void ClientToServerApplyUpgrades::execute(ServerContext& ctx) {
    std::cout << "Cliente con id: " << client_id << " está aplicando mejoras al auto" << std::endl;

    // Validar upgrades
    if (!ctx.players || !upgrades.isValid()) {
        std::cerr << "Error: Upgrades inválidos para cliente " << client_id << std::endl;
        return;
    }

    // Encontrar el jugador correspondiente al cliente
    auto it = std::find_if(ctx.players->begin(), ctx.players->end(),
                           [this](const auto& player) { return player->getId() == client_id; });

    if (it != ctx.players->end()) {
        (*it)->applyCarUpgrades(upgrades);
        std::cout << "Mejoras aplicadas para cliente " << client_id
                  << " - Penalización de tiempo: " << upgrades.getTimePenalty() << "s" << std::endl;
    }

    std::cerr << "Error: No se encontró al jugador con id " << client_id << std::endl;
}

ClientToServerApplyUpgrades* ClientToServerApplyUpgrades::from_bytes(
        const std::vector<uint8_t>& data, const int client_id) {
    // Estructura esperada: [command_type (1 byte) + 4 floats = 17 bytes mínimo]
    if (data.size() < 17) {
        throw std::runtime_error("ApplyUpgradesCmd: datos insuficientes (esperados 17 bytes)");
    }

    CarUpgrades upgrades;

    // Deserializar los 4 floats (cada uno es 4 bytes)
    std::memcpy(&upgrades.acceleration_boost, data.data() + 1, sizeof(float));
    std::memcpy(&upgrades.speed_boost, data.data() + 5, sizeof(float));
    std::memcpy(&upgrades.handling_boost, data.data() + 9, sizeof(float));
    std::memcpy(&upgrades.health_boost, data.data() + 13, sizeof(float));

    return new ClientToServerApplyUpgrades(client_id, upgrades);
}

std::vector<uint8_t> ClientToServerApplyUpgrades::to_bytes() const {
    std::vector<uint8_t> result;
    result.push_back(APPLY_UPGRADES_COMMAND);

    // Serializar los 4 floats
    uint8_t buffer[4];

    std::memcpy(buffer, &upgrades.acceleration_boost, sizeof(float));
    result.insert(result.end(), buffer, buffer + sizeof(float));

    std::memcpy(buffer, &upgrades.speed_boost, sizeof(float));
    result.insert(result.end(), buffer, buffer + sizeof(float));

    std::memcpy(buffer, &upgrades.handling_boost, sizeof(float));
    result.insert(result.end(), buffer, buffer + sizeof(float));

    std::memcpy(buffer, &upgrades.health_boost, sizeof(float));
    result.insert(result.end(), buffer, buffer + sizeof(float));

    return result;
}
