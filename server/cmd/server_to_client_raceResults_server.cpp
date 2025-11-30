#include "server_to_client_raceResults_server.h"

#include <cstring>
#include <iomanip>

#include <arpa/inet.h>

#include "../../common/buffer_utils.h"

ServerToClientRaceResults_Server::ServerToClientRaceResults_Server(
        const std::vector<PlayerResult>& playerResults, bool finished):
        results(playerResults), isFinished(finished) {}

std::vector<uint8_t> ServerToClientRaceResults_Server::to_bytes() const {
    std::vector<uint8_t> data;

    BufferUtils::append_uint8(data, SERVER_TO_CLIENT_RACE_RESULTS);

    uint8_t finishedFlag = isFinished ? 1 : 0;
    BufferUtils::append_uint8(data, finishedFlag);

    BufferUtils::append_uint8(data, static_cast<uint8_t>(results.size()));

    for (const auto& result: results) {
        BufferUtils::append_uint32(data, result.playerId);

        BufferUtils::append_uint16(data, static_cast<uint16_t>(result.playerName.length()));
        BufferUtils::append_bytes(data, result.playerName.data(), result.playerName.length());

        BufferUtils::append_float(data, result.finishTime);

        BufferUtils::append_bytes(data, &result.position, sizeof(result.position));
    }

    return data;
}

ServerToClientCmd_Server* ServerToClientRaceResults_Server::clone() const {
    return new ServerToClientRaceResults_Server(results, isFinished);
}
