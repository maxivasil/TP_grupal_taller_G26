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
        auto snapshot = ServerToClientSnapshot_Client::from_bytes(data);
        return new ServerToClientSnapshot_Client(std::move(snapshot));
    };

    registry[JOIN_RESPONSE_COMMAND] = [](const std::vector<uint8_t>& data) {
        auto joinLobbyResponse = ServerToClientJoinLobbyResponse_Client::from_bytes(data);
        return new ServerToClientJoinLobbyResponse_Client(std::move(joinLobbyResponse));
    };


    registry[GAME_STARTING_COMMAND] = [](const std::vector<uint8_t>& data) {
        auto gameStarting = ServerToClientGameStarting_Client::from_bytes(data);
        return new ServerToClientGameStarting_Client(std::move(gameStarting));
    };

    registry[SERVER_TO_CLIENT_RACE_RESULTS] = [](const std::vector<uint8_t>& data) {
        auto raceResults = ServerToClientRaceResults_Client::from_bytes(data);
        return new ServerToClientRaceResults_Client(std::move(raceResults));
    };

    registry[ASSIGN_ID_COMMAND] = [](const std::vector<uint8_t>& data) {
        auto assignId = ServerToClientAssignId_Client::from_bytes(data);
        return new ServerToClientAssignId_Client(std::move(assignId));
    };

    registry[STARTING_RACE_COMMAND] = [](const std::vector<uint8_t>& data) {
        auto startingRace = ServerToClientStartingRace_Client::from_bytes(data);
        return new ServerToClientStartingRace_Client(std::move(startingRace));
    };

    registry[ACCUMULATED_RESULTS_COMMAND] = [](const std::vector<uint8_t>& data) {
        auto accumulatedResults = ServerToClientAccumulatedResults_Client::from_bytes(data);
        return new ServerToClientAccumulatedResults_Client(std::move(accumulatedResults));
    };

    return registry;
}
