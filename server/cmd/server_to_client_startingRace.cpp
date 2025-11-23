#include "server_to_client_startingRace.h"

ServerToClientStartingRace::ServerToClientStartingRace(uint8_t cityId, std::string& trackFile):
        cityId(cityId), trackFile(trackFile) {}

std::vector<uint8_t> ServerToClientStartingRace::to_bytes() const {
    std::vector<uint8_t> data;

    BufferUtils::append_uint8(data, STARTING_RACE_COMMAND);

    BufferUtils::append_uint8(data, cityId);

    BufferUtils::append_bytes(data, trackFile.data(), trackFile.size());
    return data;
}

ServerToClientCmd_Server* ServerToClientStartingRace::clone() const {
    return new ServerToClientStartingRace(*this);
}
