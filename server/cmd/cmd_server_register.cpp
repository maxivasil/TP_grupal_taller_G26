#include "cmd_server_register.h"

ServerRegisteredCommands::ServerRegisteredCommands():
        send_registry(build_server_send_command_registry()),
        recv_registry(build_server_recv_command_registry()) {}

const std::unordered_map<uint8_t,
                         std::function<ClientToServerCmd_Server*(const std::vector<uint8_t>&)>>&
        ServerRegisteredCommands::get_send_registry() const {
    return send_registry;
}

const std::unordered_map<uint8_t,
                         std::function<ClientToServerCmd_Server*(const std::vector<uint8_t>&)>>&
        ServerRegisteredCommands::get_recv_registry() const {
    return recv_registry;
}

std::unordered_map<uint8_t, std::function<ClientToServerCmd_Server*(const std::vector<uint8_t>&)>>
        ServerRegisteredCommands::build_server_send_command_registry() {
    std::unordered_map<uint8_t,
                       std::function<ClientToServerCmd_Server*(const std::vector<uint8_t>&)>>
            registry;

    // registry[ANOTHER_COMMAND] = [](const std::vector<uint8_t>& data) {
    //     return new AnotherCommand(/* parámetros */);
    // };

    return registry;
}

std::unordered_map<uint8_t, std::function<ClientToServerCmd_Server*(const std::vector<uint8_t>&)>>
        ServerRegisteredCommands::build_server_recv_command_registry() {
    std::unordered_map<uint8_t,
                       std::function<ClientToServerCmd_Server*(const std::vector<uint8_t>&)>>
            registry;

    registry[MOVE_COMMAND] = [](const std::vector<uint8_t>& data) {
        // data[1] contiene la dirección, por ejemplo
        uint8_t dir = static_cast<uint8_t>(data[1]);
        return new ClientToServerMove(dir);
    };

    return registry;
}
