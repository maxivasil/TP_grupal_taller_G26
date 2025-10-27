#include "cmd_client_register.h"

ClientRegisteredCommands::ClientRegisteredCommands():
        send_registry(build_client_send_command_registry()),
        recv_registry(build_client_recv_command_registry()) {}

const std::unordered_map<uint8_t, std::function<ClientToServerCmd_Client*(const ParsedCommand&)>>&
        ClientRegisteredCommands::get_send_registry() const {
    return send_registry;
}

const std::unordered_map<uint8_t,
                         std::function<ServerToClientCmd_Client*(const std::vector<uint8_t>&)>>&
        ClientRegisteredCommands::get_recv_registry() const {
    return recv_registry;
}

std::unordered_map<uint8_t, std::function<ClientToServerCmd_Client*(const ParsedCommand&)>>
        ClientRegisteredCommands::build_client_send_command_registry() {
    std::unordered_map<uint8_t, std::function<ClientToServerCmd_Client*(const ParsedCommand&)>>
            registry;

    registry[MOVE_COMMAND] = [](const ParsedCommand& parsed) {
        return new ClientToServerMove(static_cast<uint8_t>(parsed.direction));
    };

    // registry[ANOTHER_COMMAND] = []() { return new AnotherCommand(); };

    return registry;
}

std::unordered_map<uint8_t, std::function<ServerToClientCmd_Client*(const std::vector<uint8_t>&)>>
        ClientRegisteredCommands::build_client_recv_command_registry() {
    std::unordered_map<uint8_t,
                       std::function<ServerToClientCmd_Client*(const std::vector<uint8_t>&)>>
            registry;

    // Agregá más comandos aquí
    // registry[ANOTHER_COMMAND] = [](const std::vector<uint8_t>& data) { return new
    // AnotherCommand(data); };

    return registry;
}
