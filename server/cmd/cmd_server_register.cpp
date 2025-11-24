#include "cmd_server_register.h"

#include "client_to_server_applyUpgrades.h"
#include "client_to_server_cheat.h"
#include "client_to_server_joinLobby.h"
#include "client_to_server_move.h"
#include "client_to_server_readyToStart.h"
#include "client_to_server_tour.h"

ServerRegisteredCommands::ServerRegisteredCommands():
        recv_registry(build_server_recv_command_registry()) {}

const std::unordered_map<uint8_t, std::function<ClientToServerCmd_Server*(
                                          const std::vector<uint8_t>&, const uint32_t client_id)>>&
        ServerRegisteredCommands::get_recv_registry() const {
    return recv_registry;
}

std::unordered_map<uint8_t, std::function<ClientToServerCmd_Server*(const std::vector<uint8_t>&,
                                                                    const uint32_t client_id)>>
        ServerRegisteredCommands::build_server_recv_command_registry() {
    std::unordered_map<uint8_t, std::function<ClientToServerCmd_Server*(const std::vector<uint8_t>&,
                                                                        const uint32_t client_id)>>
            registry;

    registry[MOVE_COMMAND] = [](const std::vector<uint8_t>& data, const uint32_t client_id) {
        return ClientToServerMove::from_bytes(data, client_id);
    };

    registry[JOIN_COMMAND] = [](const std::vector<uint8_t>& data, const uint32_t client_id) {
        return ClientToServerJoinLobby::from_bytes(data, client_id);
    };

    registry[READY_TO_START_COMMAND] = [](const std::vector<uint8_t>& data,
                                          const uint32_t client_id) {
        return ClientToServerReady::from_bytes(data, client_id);
    };

    registry[CHEAT_COMMAND] = [](const std::vector<uint8_t>& data, const uint32_t client_id) {
        return ClientToServerCheat::from_bytes(data, client_id);
    };

    registry[TOUR_COMMAND] = [](const std::vector<uint8_t>& data, const uint32_t client_id) {
        return ClientToServerTour::from_bytes(data, client_id);
    };

    registry[APPLY_UPGRADES_COMMAND] = [](const std::vector<uint8_t>& data,
                                          const uint32_t client_id) {
        return ClientToServerApplyUpgrades::from_bytes(data, client_id);
    };

    return registry;
}
