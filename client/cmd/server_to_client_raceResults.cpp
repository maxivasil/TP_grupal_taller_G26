#include "server_to_client_raceResults.h"
#include <cstring>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <arpa/inet.h>
#include "../client_game.h"

ServerToClientRaceResults::ServerToClientRaceResults(std::vector<ClientPlayerResult> playerResults, bool finished)
    : results(std::move(playerResults)), isFinished(finished) {}

ServerToClientRaceResults ServerToClientRaceResults::from_bytes(const std::vector<uint8_t>& data) {
    if (data.empty()) {
        throw std::runtime_error("Race results data is empty");
    }

    std::cout << "\n=== CLIENT: Deserializing Race Results ===" << std::endl;
    std::cout << "Received " << data.size() << " bytes" << std::endl;
    std::cout << "First byte (command): " << static_cast<int>(data[0]) << " (should be 0x08 = " << SERVER_TO_CLIENT_RACE_RESULTS << ")" << std::endl;

    size_t offset = 0;

    uint8_t header = data[offset++];
    if (header != SERVER_TO_CLIENT_RACE_RESULTS)
        throw std::runtime_error("Invalid header for race results");

    if (offset >= data.size())
        throw std::runtime_error("Missing finished flag in race results");
    
    bool isFinished = data[offset++] != 0;
    std::cout << "Is finished flag: " << (isFinished ? "YES" : "NO") << std::endl;

    if (offset >= data.size())
        throw std::runtime_error("Missing result count in race results");
    
    uint8_t numResults = data[offset++];
    std::cout << "Number of results: " << static_cast<int>(numResults) << std::endl;
    
    std::vector<ClientPlayerResult> results;
    
    for (uint8_t i = 0; i < numResults; ++i) {
        ClientPlayerResult result;
        
        if (offset >= data.size())
            throw std::runtime_error("Incomplete race results: missing player ID");
        result.playerId = data[offset++];
        
        if (offset + sizeof(uint16_t) > data.size())
            throw std::runtime_error("Incomplete race results: missing name length");
        
        uint16_t nameLen;
        std::memcpy(&nameLen, &data[offset], sizeof(uint16_t));
        offset += sizeof(uint16_t);
        
        if (offset + nameLen > data.size())
            throw std::runtime_error("Incomplete race results: missing player name");
        result.playerName = std::string((const char*)&data[offset], nameLen);
        offset += nameLen;
        
        if (offset + sizeof(float) > data.size())
            throw std::runtime_error("Incomplete race results: missing finish time");
        std::memcpy(&result.finishTime, &data[offset], sizeof(float));
        offset += sizeof(float);
        
        if (offset >= data.size())
            throw std::runtime_error("Incomplete race results: missing position");
        result.position = data[offset++];
        
        std::cout << "  Result " << static_cast<int>(i) << ": " << result.playerName 
                  << " (ID:" << static_cast<int>(result.playerId) << ") Position:" 
                  << static_cast<int>(result.position) << " Time:" << result.finishTime << "s" << std::endl;
        
        results.push_back(result);
    }
    
    std::cout << "==============================\n" << std::endl;
    return ServerToClientRaceResults(results, isFinished);
}

void ServerToClientRaceResults::execute(ClientContext& ctx) {
    std::cout << "\n=== CLIENT: Executing Race Results Command ===" << std::endl;
    std::cout << "Results received: " << results.size() << " players" << std::endl;
    std::cout << "Race Finished: " << (isFinished ? "YES" : "NO") << std::endl;
    std::cout << "Game context available: " << (ctx.game ? "YES" : "NO") << std::endl;
    
    std::cout << "\nPlayer Rankings:" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    for (const auto& result : results) {
        int minutes = static_cast<int>(result.finishTime) / 60;
        int seconds = static_cast<int>(result.finishTime) % 60;
        int millis = static_cast<int>((result.finishTime - static_cast<int>(result.finishTime)) * 1000);
        
        std::cout << std::setw(2) << static_cast<int>(result.position) << ". " 
                  << std::setw(20) << std::left << result.playerName 
                  << " - " << std::setw(2) << std::right << minutes << ":" 
                  << std::setfill('0') << std::setw(2) << seconds << "."
                  << std::setw(3) << millis << std::setfill(' ') << std::endl;
    }
    std::cout << std::string(50, '=') << std::endl;
    
    if (ctx.game) {
        std::cout << "Calling ctx.game->setRaceResults()..." << std::endl;
        ctx.game->setRaceResults(results);
        std::cout << "setRaceResults() called successfully!" << std::endl;
    } else {
        std::cerr << "ERROR: ctx.game is nullptr!" << std::endl;
    }
}
