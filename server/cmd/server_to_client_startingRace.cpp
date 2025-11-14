#include "server_to_client_startingRace.h"

ServerToClientStartingRace::ServerToClientStartingRace() {}

std::vector<uint8_t> ServerToClientStartingRace::to_bytes() const {
    std::vector<uint8_t> data;

    uint8_t header = STARTING_RACE_COMMAND;
    data.push_back(header);

    return data;
}

ServerToClientCmd_Server* ServerToClientStartingRace::clone() const {
    return new ServerToClientStartingRace(*this);
}
