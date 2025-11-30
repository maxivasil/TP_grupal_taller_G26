#include "server_to_client_accumulatedResults_server.h"

#include <cstring>

#include <arpa/inet.h>

ServerToClientAccumulatedResults_Server::ServerToClientAccumulatedResults_Server(
        std::vector<AccumulatedResultDTO> results):
        results(std::move(results)) {}

std::vector<uint8_t> ServerToClientAccumulatedResults_Server::to_bytes() const {
    std::vector<uint8_t> bytes;

    BufferUtils::append_uint8(bytes, ACCUMULATED_RESULTS_COMMAND);

    BufferUtils::append_uint16(bytes, static_cast<uint16_t>(results.size()));

    for (const auto& dto: results) {
        BufferUtils::append_uint32(bytes, dto.playerId);

        BufferUtils::append_uint16(bytes, static_cast<uint16_t>(dto.playerName.length()));
        BufferUtils::append_bytes(bytes, dto.playerName.data(), dto.playerName.length());

        BufferUtils::append_uint16(bytes, dto.completedRaces);

        BufferUtils::append_float(bytes, dto.totalTime);
    }

    return bytes;
}

ServerToClientCmd_Server* ServerToClientAccumulatedResults_Server::clone() const {
    return new ServerToClientAccumulatedResults_Server(*this);
}
