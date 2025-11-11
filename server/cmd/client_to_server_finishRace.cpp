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
    auto& finishTimes = const_cast<std::unordered_map<int, float>&>(ctx.race->getFinishTimes());
    
    // Check if already registered
    if (finishTimes.count(client_id)) {
        std::cout << "[SERVER] Client " << client_id << " already registered as finished" << std::endl;
        return;
    }
    
    // Find player and calculate time
    for (const auto& player : ctx.race->getPlayers()) {
        if (player->getId() == client_id) {
            // Calculate the elapsed time from race start to now
            float finishTime = ctx.race->getCurrentElapsedTime();
            
            std::cout << "[SERVER] Registering client " << client_id << " (" << player->getName() 
                      << ") as finished in " << finishTime << "s!" << std::endl;
            
            finishTimes[client_id] = finishTime;
            break;
        }
    }
}



