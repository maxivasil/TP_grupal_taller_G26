#include "client_to_server_finishRace.h"
#include "../game_logic/Race.h"
#include <iostream>
#include <chrono>

ClientToServerFinishRace::ClientToServerFinishRace(int client_id) 
    : ClientToServerCmd_Server(client_id), client_id(client_id) {}

ClientToServerFinishRace* ClientToServerFinishRace::from_bytes(const std::vector<uint8_t>& data, int client_id) {
    std::cout << "[SERVER] Received finish race notification from client " << client_id << std::endl;
    return new ClientToServerFinishRace(client_id);
}

void ClientToServerFinishRace::execute(ServerContext& ctx) {
    if (!ctx.race) {
        std::cerr << "[SERVER] ERROR: No race context!" << std::endl;
        return;
    }
    
    // Record the finish time for this player
    // Access the race's internal data to register this player's finish time
    // Since we don't have direct access to startTime, we'll let the race calculate it
    // by checking if the player is already registered
    
    auto& finishTimes = const_cast<std::unordered_map<int, float>&>(ctx.race->getFinishTimes());
    
    // Check if already registered
    if (finishTimes.count(client_id)) {
        std::cout << "[SERVER] Client " << client_id << " already registered as finished" << std::endl;
        return;
    }
    
    // Find player and calculate time
    for (const auto& player : ctx.race->getPlayers()) {
        if (player->getId() == client_id) {
            // We don't have access to start time here, so we'll set a placeholder
            // The actual time will be calculated when needed
            std::cout << "[SERVER] Registering client " << client_id << " (" << player->getName() 
                      << ") as finished!" << std::endl;
            finishTimes[client_id] = 0.0f;  // Placeholder - will be updated by checkFinishConditions
            break;
        }
    }
}

