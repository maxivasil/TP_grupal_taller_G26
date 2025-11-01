#include "cmd_server_register.h"

ServerRegisteredCommands::ServerRegisteredCommands():
        recv_registry(build_server_recv_command_registry()) {}

const std::unordered_map<uint8_t, std::function<ClientToServerCmd_Server*(
                                          const std::vector<uint8_t>&, const int client_id)>>&
        ServerRegisteredCommands::get_recv_registry() const {
    return recv_registry;
}

std::unordered_map<uint8_t, std::function<ClientToServerCmd_Server*(const std::vector<uint8_t>&,
                                                                    const int client_id)>>
        ServerRegisteredCommands::build_server_recv_command_registry() {
    std::unordered_map<uint8_t, std::function<ClientToServerCmd_Server*(const std::vector<uint8_t>&,
                                                                        const int client_id)>>
            registry;

    registry[MOVE_COMMAND] = [](const std::vector<uint8_t>& data, const int client_id) {
        return ClientToServerMove::from_bytes(data, client_id);
    };

    return registry;
}
