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

    registry[JOIN_RESPONSE_COMMAND] = [](const std::vector<uint8_t>& data) {
        auto snapshot = ServerToClientLobbyResponse::from_bytes(data);
        return new ServerToClientLobbyResponse(std::move(snapshot));
    };


    registry[GAME_STARTING_COMMAND] = [](const std::vector<uint8_t>& data) {
        auto snapshot = ServerToClientGameStarting::from_bytes(data);
        return new ServerToClientGameStarting(std::move(snapshot));
    };

    registry[SERVER_TO_CLIENT_RACE_RESULTS] = [](const std::vector<uint8_t>& data) {
        auto raceResults = ServerToClientRaceResults::from_bytes(data);
        return new ServerToClientRaceResults(std::move(raceResults));
    };

    registry[ASSIGN_ID_COMMAND] = [](const std::vector<uint8_t>& data) {
        auto assignId = ServerToClientAssignId::from_bytes(data);
        return new ServerToClientAssignId(std::move(assignId));
    };

    return registry;
}
