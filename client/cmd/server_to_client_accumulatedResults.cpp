#include "server_to_client_accumulatedResults.h"

#include "../client_game.h"

ServerToClientAccumulatedResults::ServerToClientAccumulatedResults(
        const std::vector<AccumulatedResultDTO>& res):
        results(res) {}

void ServerToClientAccumulatedResults::execute(ClientContext& ctx) {
    ctx.game->setAccumulatedResults(results);
}

ServerToClientAccumulatedResults ServerToClientAccumulatedResults::from_bytes(
        const std::vector<uint8_t>& data) {
    size_t offset = 0;

    if (data[offset] != ACCUMULATED_RESULTS_COMMAND)
        throw std::runtime_error("Invalid command type for AccumulatedResults");
    offset += 1;

    uint16_t count;
    memcpy(&count, &data[offset], sizeof(uint16_t));
    count = ntohs(count);
    offset += sizeof(uint16_t);

    std::vector<AccumulatedResultDTO> res;
    res.reserve(count);

    for (int i = 0; i < count; i++) {
        AccumulatedResultDTO dto;

        uint32_t playerId;
        memcpy(&playerId, &data[offset], sizeof(uint32_t));
        playerId = ntohl(playerId);
        dto.playerId = playerId;
        offset += sizeof(uint32_t);

        uint16_t races;
        memcpy(&races, &data[offset], sizeof(uint16_t));
        races = ntohs(races);
        dto.completedRaces = races;
        offset += sizeof(uint16_t);

        uint32_t timeBits;
        memcpy(&timeBits, &data[offset], sizeof(uint32_t));
        timeBits = ntohl(timeBits);

        float totalTime;
        memcpy(&totalTime, &timeBits, sizeof(float));
        dto.totalTime = totalTime;
        offset += sizeof(uint32_t);

        res.push_back(dto);
    }

    return ServerToClientAccumulatedResults(res);
}
