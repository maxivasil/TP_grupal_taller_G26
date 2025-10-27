#include "cmd_client_register.h"

ClientRegisteredCommands::ClientRegisteredCommands():
        recv_registry(build_client_recv_command_registry()) {}

const std::unordered_map<uint8_t,
                         std::function<ServerToClientCmd_Client*(const std::vector<uint8_t>&)>>&
        ClientRegisteredCommands::get_recv_registry() const {
    return recv_registry;
}

std::unordered_map<uint8_t, std::function<ServerToClientCmd_Client*(const std::vector<uint8_t>&)>>
        ClientRegisteredCommands::build_client_recv_command_registry() {
    std::unordered_map<uint8_t,
                       std::function<ServerToClientCmd_Client*(const std::vector<uint8_t>&)>>
            registry;

    registry[SNAPSHOT_COMMAND] = [](const std::vector<uint8_t>& data) {
        auto snapshot = ServerToClientSnapshot::from_bytes(data);
        return new ServerToClientSnapshot(std::move(snapshot));
    };

    return registry;
}
