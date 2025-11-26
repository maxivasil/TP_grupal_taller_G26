#include "server_to_client_startingRace_server.h"

ServerToClientStartingRace_Server::ServerToClientStartingRace_Server(uint8_t cityId,
                                                                     std::string& trackFile):
        cityId(cityId), trackFile(trackFile) {}

std::vector<uint8_t> ServerToClientStartingRace_Server::to_bytes() const {
    std::vector<uint8_t> data;

    BufferUtils::append_uint8(data, STARTING_RACE_COMMAND);

    BufferUtils::append_uint8(data, cityId);

    BufferUtils::append_bytes(data, trackFile.data(), trackFile.size());
    return data;
}

ServerToClientCmd_Server* ServerToClientStartingRace_Server::clone() const {
    return new ServerToClientStartingRace_Server(*this);
}
