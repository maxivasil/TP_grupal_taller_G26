#include "server_to_client_startingRace_server.h"

ServerToClientStartingRace_Server::ServerToClientStartingRace_Server(uint8_t cityId,
                                                                     std::string& trackFile,
                                                                     bool isLastRace,
                                                                     uint8_t countdownValue):
        cityId(cityId),
        trackFile(trackFile),
        isLastRace(isLastRace),
        countdownValue(countdownValue) {}

std::vector<uint8_t> ServerToClientStartingRace_Server::to_bytes() const {
    std::vector<uint8_t> data;

    BufferUtils::append_uint8(data, STARTING_RACE_COMMAND);

    BufferUtils::append_uint8(data, cityId);

    uint8_t lastRaceFlag = isLastRace ? 1 : 0;
    BufferUtils::append_uint8(data, lastRaceFlag);

    BufferUtils::append_uint8(data, countdownValue);

    BufferUtils::append_bytes(data, trackFile.data(), trackFile.size());
    return data;
}

ServerToClientCmd_Server* ServerToClientStartingRace_Server::clone() const {
    return new ServerToClientStartingRace_Server(*this);
}
