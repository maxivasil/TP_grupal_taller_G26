#include "server_to_client_accumulatedResults_server.h"

#include <cstring>

#include <arpa/inet.h>

ServerToClientAccumulatedResults_Server::ServerToClientAccumulatedResults_Server(
        std::vector<AccumulatedResultDTO> results, bool isLastRace):
        results(std::move(results)), isLastRace(isLastRace) {}

std::vector<uint8_t> ServerToClientAccumulatedResults_Server::to_bytes() const {
    std::vector<uint8_t> bytes;

    BufferUtils::append_uint8(bytes, ACCUMULATED_RESULTS_COMMAND);

    uint8_t lastRaceFlag = isLastRace ? 1 : 0;
    BufferUtils::append_uint8(bytes, lastRaceFlag);

    BufferUtils::append_uint16(bytes, static_cast<uint16_t>(results.size()));

    for (const auto& dto: results) {
        BufferUtils::append_uint32(bytes, dto.playerId);

        BufferUtils::append_uint16(bytes, dto.completedRaces);

        BufferUtils::append_float(bytes, dto.totalTime);
    }

    return bytes;
}

ServerToClientCmd_Server* ServerToClientAccumulatedResults_Server::clone() const {
    return new ServerToClientAccumulatedResults_Server(*this);
}
