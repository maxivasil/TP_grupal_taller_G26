#include "server_to_client_startingRace.h"

#include <cstring>
#include <stdexcept>

#include "../client_game.h"

ServerToClientStartingRace::ServerToClientStartingRace() {}

void ServerToClientStartingRace::execute(ClientContext& ctx) { ctx.game->resetForNextRace(); }

ServerToClientStartingRace ServerToClientStartingRace::from_bytes(
        const std::vector<uint8_t>& data) {
    if (data.empty()) {
        throw std::runtime_error("Race Starting data is empty");
    }
    uint8_t header = data[0];
    if (header != STARTING_RACE_COMMAND)
        throw std::runtime_error("Invalid header for Race Starting");
    return ServerToClientStartingRace();
}
