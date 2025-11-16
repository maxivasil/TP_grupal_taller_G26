#include "server_to_client_startingRace.h"

#include <cstring>
#include <stdexcept>

#include "../client_game.h"

ServerToClientStartingRace::ServerToClientStartingRace(uint8_t cityId): cityId(cityId) {}

void ServerToClientStartingRace::execute(ClientContext& ctx) { ctx.game->resetForNextRace(cityId); }

ServerToClientStartingRace ServerToClientStartingRace::from_bytes(
        const std::vector<uint8_t>& data) {
    if (data.empty()) {
        throw std::runtime_error("Race Starting data is empty");
    }
    if (data.size() < 2) {
        throw std::runtime_error("Insufficient data for Race Starting");
    }
    uint8_t header = data[0];
    if (header != STARTING_RACE_COMMAND)
        throw std::runtime_error("Invalid header for Race Starting");
    uint8_t cityId;
    std::memcpy(&cityId, &data[1], sizeof(cityId));
    return ServerToClientStartingRace(cityId);
}
