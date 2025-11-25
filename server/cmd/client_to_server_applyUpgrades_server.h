#ifndef CLIENT_TO_SERVER_APPLY_UPGRADES_SERVER_H
#define CLIENT_TO_SERVER_APPLY_UPGRADES_SERVER_H

#include <cstdint>
#include <vector>

#include "../../common/constants.h"
#include "../../server/game_logic/CarUpgrades.h"

#include "cmd_base_server.h"

/**
 * @class ClientToServerApplyUpgrades
 * @brief Comando para aplicar mejoras al auto de un jugador.
 *
 * Protocolo:
 * [0] command_type (1 byte) = APPLY_UPGRADES_COMMAND
 * [1-4] acceleration_boost (4 bytes, float)
 * [5-8] speed_boost (4 bytes, float)
 * [9-12] handling_boost (4 bytes, float)
 * [13-16] health_boost (4 bytes, float)
 */
class ClientToServerApplyUpgrades_Server: public ClientToServerCmd_Server {
public:
    ClientToServerApplyUpgrades_Server(uint32_t client_id, const CarUpgrades& upgrades);

    void execute(ServerContext& ctx) override;

    static ClientToServerApplyUpgrades_Server* from_bytes(const std::vector<uint8_t>& data,
                                                          const uint32_t client_id);

    const CarUpgrades get_only_for_test_upgrades() const;

private:
    CarUpgrades upgrades;
};

#endif  // CLIENT_TO_SERVER_APPLY_UPGRADES_SERVER_H
