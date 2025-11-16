#include "server_to_client_startingRace.h"

ServerToClientStartingRace::ServerToClientStartingRace(uint8_t cityId, std::string& trackFile):
        cityId(cityId), trackFile(trackFile) {}

std::vector<uint8_t> ServerToClientStartingRace::to_bytes() const {
    std::vector<uint8_t> data;

    uint8_t header = STARTING_RACE_COMMAND;
    data.push_back(header);

    uint8_t mapId = cityId;
    BufferUtils::append_bytes(data, &mapId, sizeof(mapId));

    BufferUtils::append_bytes(data, trackFile.data(), trackFile.size());
    return data;
}

ServerToClientCmd_Server* ServerToClientStartingRace::clone() const {
    return new ServerToClientStartingRace(*this);
}
