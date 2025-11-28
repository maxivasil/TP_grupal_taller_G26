#include "server_to_client_accumulatedResults_client.h"

#include "../client_game.h"

ServerToClientAccumulatedResults_Client::ServerToClientAccumulatedResults_Client(
        const std::vector<AccumulatedResultDTO>& res, bool isLastRace):
        results(res), isLastRace(isLastRace) {}

void ServerToClientAccumulatedResults_Client::execute(ClientContext& ctx) {
    ctx.game->setAccumulatedResults(results, isLastRace);
}

ServerToClientAccumulatedResults_Client ServerToClientAccumulatedResults_Client::from_bytes(
        const std::vector<uint8_t>& data) {
    size_t offset = 0;

    if (data[offset] != ACCUMULATED_RESULTS_COMMAND)
        throw std::runtime_error("Invalid command type for AccumulatedResults");
    offset += 1;

    uint8_t isLastRace;
    BufferUtils::read_uint8(data, offset, isLastRace);

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

    return ServerToClientAccumulatedResults_Client(res, isLastRace == 1);
}

const std::vector<AccumulatedResultDTO>
        ServerToClientAccumulatedResults_Client::get_only_for_test_results() const {
    return results;
}

bool ServerToClientAccumulatedResults_Client::get_only_for_test_isLastRace() const {
    return isLastRace;
}
