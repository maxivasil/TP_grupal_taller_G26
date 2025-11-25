#include "server_to_client_raceResults_client.h"

#include <cstring>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <utility>

#include <arpa/inet.h>

#include "../client_game.h"

ServerToClientRaceResults_Client::ServerToClientRaceResults_Client(
        std::vector<ClientPlayerResult> playerResults, bool finished):
        results(std::move(playerResults)), isFinished(finished) {}

ServerToClientRaceResults_Client ServerToClientRaceResults_Client::from_bytes(
        const std::vector<uint8_t>& data) {
    if (data.empty()) {
        throw std::runtime_error("Race results data is empty");
    }

    size_t offset = 0;

    uint8_t header = data[offset++];
    if (header != SERVER_TO_CLIENT_RACE_RESULTS)
        throw std::runtime_error("Invalid header for race results");

    if (offset >= data.size())
        throw std::runtime_error("Missing finished flag in race results");

    bool isFinished = data[offset++] != 0;

    if (offset >= data.size())
        throw std::runtime_error("Missing result count in race results");

    uint8_t numResults = data[offset++];

    std::vector<ClientPlayerResult> results;

    for (uint8_t i = 0; i < numResults; ++i) {
        ClientPlayerResult result;

        if (offset >= data.size())
            throw std::runtime_error("Incomplete race results: missing player ID");

        BufferUtils::read_uint32(data, offset, result.playerId);

        if (offset + sizeof(uint16_t) > data.size())
            throw std::runtime_error("Incomplete race results: missing name length");

        uint16_t nameLen;
        BufferUtils::read_uint16(data, offset, nameLen);

        if (offset + nameLen > data.size())
            throw std::runtime_error("Incomplete race results: missing player name");

        result.playerName = std::string(reinterpret_cast<const char*>(&data[offset]), nameLen);
        offset += nameLen;

        if (offset + sizeof(float) > data.size())
            throw std::runtime_error("Incomplete race results: missing finish time");

        BufferUtils::read_float(data, offset, result.finishTime);

        if (offset >= data.size())
            throw std::runtime_error("Incomplete race results: missing position");

        result.position = data[offset++];

        results.push_back(result);
    }

    return ServerToClientRaceResults_Client(results, isFinished);
}

void ServerToClientRaceResults_Client::execute(ClientContext& ctx) {
    if (ctx.game) {
        ctx.game->setRaceResults(results, isFinished);
    } else {
        std::cerr << "ERROR: ctx.game is nullptr!" << std::endl;
    }
}

const std::vector<ClientPlayerResult>& ServerToClientRaceResults_Client::get_only_for_test_results()
        const {
    return results;
}

bool ServerToClientRaceResults_Client::get_only_for_test_isFinished() const { return isFinished; }
