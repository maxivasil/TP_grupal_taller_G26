#include "server_to_client_accumulatedResults.h"

#include <cstring>

#include <arpa/inet.h>

ServerToClientAccumulatedResults::ServerToClientAccumulatedResults(
        std::unordered_map<int, AccumulatedInfo> accumulatedResults):
        accumulatedResults(std::move(accumulatedResults)) {}

std::vector<uint8_t> ServerToClientAccumulatedResults::to_bytes() const {
    std::vector<uint8_t> bytes;

    // 1) Tipo de comando
    uint8_t cmd = static_cast<uint8_t>(ACCUMULATED_RESULTS_COMMAND);
    BufferUtils::append_bytes(bytes, &cmd, sizeof(cmd));

    // 2) Tamaño del mapa (network order)
    uint16_t mapSize = htons(static_cast<uint16_t>(accumulatedResults.size()));
    BufferUtils::append_bytes(bytes, &mapSize, sizeof(mapSize));

    // 3) Entradas
    for (const auto& [playerId, info]: accumulatedResults) {

        // --- playerId ---
        uint32_t pid = htonl(static_cast<uint32_t>(playerId));
        BufferUtils::append_bytes(bytes, &pid, sizeof(pid));

        // --- completedRaces ---
        uint32_t races = htonl(static_cast<uint32_t>(info.completedRaces));
        BufferUtils::append_bytes(bytes, &races, sizeof(races));

        // --- totalTime ---
        uint32_t timeBits;
        memcpy(&timeBits, &info.totalTime, sizeof(float));
        timeBits = htonl(timeBits);
        BufferUtils::append_bytes(bytes, &timeBits, sizeof(timeBits));
    }

    return bytes;
}

ServerToClientCmd_Server* ServerToClientAccumulatedResults::clone() const {
    return new ServerToClientAccumulatedResults(*this);
}
