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

    // 1) Command
    if (data[offset] != ACCUMULATED_RESULTS_COMMAND)
        throw std::runtime_error("Invalid command type");
    offset += 1;

    // 2) mapSize (network order)
    uint16_t mapSize;
    memcpy(&mapSize, &data[offset], sizeof(uint16_t));
    mapSize = ntohs(mapSize);
    offset += sizeof(uint16_t);

    std::vector<AccumulatedResultDTO> res;
    res.reserve(mapSize);

    // 3) Parse entries
    for (int i = 0; i < mapSize; i++) {
        AccumulatedResultDTO dto;

        // --- playerId ---
        uint32_t pid;
        memcpy(&pid, &data[offset], sizeof(uint32_t));
        pid = ntohl(pid);
        dto.playerId = pid;
        offset += sizeof(uint32_t);

        // --- completedRaces ---
        uint32_t races;
        memcpy(&races, &data[offset], sizeof(uint32_t));
        races = ntohl(races);
        dto.completedRaces = static_cast<int>(races);
        offset += sizeof(uint32_t);

        // --- totalTime (float) ---
        uint32_t timeBits;
        memcpy(&timeBits, &data[offset], sizeof(uint32_t));
        timeBits = ntohl(timeBits);

        float totalTime;
        memcpy(&totalTime, &timeBits, sizeof(float));
        dto.totalTime = totalTime;
        offset += sizeof(uint32_t);

        // Si querés agregar el nombre más adelante:
        dto.name = "";  // o algo por defecto

        res.push_back(dto);
    }

    return ServerToClientAccumulatedResults(res);
}
