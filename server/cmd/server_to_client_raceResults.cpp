#include "server_to_client_raceResults.h"
#include "../../common/buffer_utils.h"
#include <iostream>
#include <iomanip>

ServerToClientRaceResults::ServerToClientRaceResults()
    : isFinished(false) {}

ServerToClientRaceResults::ServerToClientRaceResults(const std::vector<PlayerResult>& playerResults, bool finished)
    : results(playerResults), isFinished(finished) {}

std::vector<uint8_t> ServerToClientRaceResults::to_bytes() const {
    std::vector<uint8_t> data;
    
    // Write command type
    uint8_t header = SERVER_TO_CLIENT_RACE_RESULTS;
    data.push_back(header);
    
    // Write finished flag
    uint8_t finishedFlag = isFinished ? 1 : 0;
    BufferUtils::append_bytes(data, &finishedFlag, sizeof(finishedFlag));
    
    // Write number of results
    uint8_t numResults = static_cast<uint8_t>(results.size());
    BufferUtils::append_bytes(data, &numResults, sizeof(numResults));
    
    // Write each result
    for (const auto& result : results) {
        BufferUtils::append_bytes(data, &result.playerId, sizeof(result.playerId));
        
        // Write string with length prefix
        uint16_t nameLen = static_cast<uint16_t>(result.playerName.length());
        BufferUtils::append_bytes(data, &nameLen, sizeof(nameLen));
        BufferUtils::append_bytes(data, (const void*)result.playerName.c_str(), nameLen);
        
        BufferUtils::append_bytes(data, &result.finishTime, sizeof(result.finishTime));
        BufferUtils::append_bytes(data, &result.position, sizeof(result.position));
    }
    
    return data;
}

ServerToClientCmd_Server* ServerToClientRaceResults::clone() const {
    return new ServerToClientRaceResults(results, isFinished);
}

