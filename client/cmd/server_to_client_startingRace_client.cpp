#include "server_to_client_startingRace_client.h"

#include <cstring>
#include <stdexcept>

#include "../client_game.h"

ServerToClientStartingRace_Client::ServerToClientStartingRace_Client(uint8_t cityId,
                                                                     std::string& trackFile,
                                                                     bool isLastRace,
                                                                     uint8_t countdownValue):
        cityId(cityId),
        trackFile(trackFile),
        isLastRace(isLastRace),
        countdownValue(countdownValue) {}

void ServerToClientStartingRace_Client::execute(ClientContext& ctx) {
    if (ctx.game) {
        if (countdownValue != UINT8_MAX) {
            ctx.game->setCountdownValue(countdownValue);
            return;
        }
        ctx.game->resetForNextRace(cityId, trackFile);
        ctx.game->setIsLastRace(isLastRace);
    }
}

ServerToClientStartingRace_Client ServerToClientStartingRace_Client::from_bytes(
        const std::vector<uint8_t>& data) {
    if (data.empty()) {
        throw std::runtime_error("Race Starting data is empty");
    }
    if (data.size() < 5) {
        throw std::runtime_error("Insufficient data for Race Starting");
    }
    uint8_t header = data[0];
    if (header != STARTING_RACE_COMMAND)
        throw std::runtime_error("Invalid header for Race Starting");

    uint8_t cityId;
    size_t offset = 1;
    BufferUtils::read_uint8(data, offset, cityId);

    bool isLastRace = false;
    uint8_t isLastRaceFlag;
    BufferUtils::read_uint8(data, offset, isLastRaceFlag);
    isLastRace = (isLastRaceFlag != 0);

    uint8_t countdownValue = 0;
    BufferUtils::read_uint8(data, offset, countdownValue);

    std::string trackFile;
    if (data.size() > offset) {
        trackFile = std::string(data.begin() + offset, data.end());
    }
    return ServerToClientStartingRace_Client(cityId, trackFile, isLastRace, countdownValue);
}

uint8_t ServerToClientStartingRace_Client::get_only_for_test_cityId() const { return cityId; }

const std::string& ServerToClientStartingRace_Client::get_only_for_test_trackFile() const {
    return trackFile;
}

bool ServerToClientStartingRace_Client::get_only_for_test_isLastRace() const { return isLastRace; }

uint8_t ServerToClientStartingRace_Client::get_only_for_test_countdownValue() const {
    return countdownValue;
}
