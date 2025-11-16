#include "server_to_client_accumulatedResults.h"

#include <cstring>

#include <arpa/inet.h>

ServerToClientAccumulatedResults::ServerToClientAccumulatedResults(
        std::vector<AccumulatedResultDTO> accumulatedResults):
        accumulatedResults(std::move(accumulatedResults)) {}

std::vector<uint8_t> ServerToClientAccumulatedResults::to_bytes() const {
    std::vector<uint8_t> bytes;

    uint8_t cmd = static_cast<uint8_t>(ACCUMULATED_RESULTS_COMMAND);
    BufferUtils::append_bytes(bytes, &cmd, sizeof(cmd));

    uint16_t count = htons(static_cast<uint16_t>(accumulatedResults.size()));
    BufferUtils::append_bytes(bytes, &count, sizeof(count));

    for (const auto& dto: accumulatedResults) {

        uint32_t pid = htonl(static_cast<uint32_t>(dto.playerId));
        BufferUtils::append_bytes(bytes, &pid, sizeof(uint32_t));

        uint16_t races = htons(dto.completedRaces);
        BufferUtils::append_bytes(bytes, &races, sizeof(races));

        uint32_t timeBits;
        memcpy(&timeBits, &dto.totalTime, sizeof(float));

        timeBits = htonl(timeBits);
        BufferUtils::append_bytes(bytes, &timeBits, sizeof(timeBits));
    }

    return bytes;
}

ServerToClientCmd_Server* ServerToClientAccumulatedResults::clone() const {
    return new ServerToClientAccumulatedResults(*this);
}
