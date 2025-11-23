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
    BufferUtils::read_uint16(data, offset, count);

    std::vector<AccumulatedResultDTO> res;
    res.reserve(count);

    for (int i = 0; i < count; i++) {
        AccumulatedResultDTO dto;

        uint32_t playerId;
        BufferUtils::read_uint32(data, offset, playerId);
        dto.playerId = playerId;

        uint16_t races;
        BufferUtils::read_uint16(data, offset, races);
        dto.completedRaces = races;

        float totalTime;
        BufferUtils::read_float(data, offset, totalTime);
        dto.totalTime = totalTime;

        res.push_back(dto);
    }

    return ServerToClientAccumulatedResults(res);
}
