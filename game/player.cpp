#include "player.h"
#include <iostream>
#include <chrono>
#include <cmath>

Player::Player() 
    : totalCheckpoints(0), 
      checkpointsCompleted(0),
      startTime(0),
      finishTime(0),
      raceFinished(false) {
}

void Player::completeCheckpoint(int checkpointId) {
    // Evitar contar el mismo checkpoint dos veces
    if (completedCheckpointIds.find(checkpointId) != completedCheckpointIds.end()) {
        return;  // Ya fue completado
    }
    
    completedCheckpointIds.insert(checkpointId);
    checkpointsCompleted++;
    
    std::cout << "✓ Checkpoint " << checkpointId << " completed! "
              << "(" << checkpointsCompleted << "/" << totalCheckpoints << ")\n";
}

void Player::finishRace() {
    if (raceFinished) return;
    
    raceFinished = true;
    finishTime = getElapsedTime();
    
    std::cout << "🏁 RACE FINISHED! Time: " << finishTime << "s\n";
}

void Player::reset() {
    checkpointsCompleted = 0;
    completedCheckpointIds.clear();
    raceFinished = false;
    startTime = 0;
    finishTime = 0;
    
    std::cout << "↻ Race reset\n";
}

float Player::getElapsedTime() const {
    static auto raceStart = std::chrono::high_resolution_clock::now();
    
    if (startTime == 0) {
        // Primera vez que se llama
        const_cast<Player*>(this)->startTime = 
            std::chrono::duration<float>(
                std::chrono::high_resolution_clock::now() - raceStart
            ).count();
    }
    
    auto now = std::chrono::high_resolution_clock::now();
    float currentTime = std::chrono::duration<float>(now - raceStart).count();
    return currentTime - startTime;
}

bool Player::hasCompletedCheckpoint(int checkpointId) const {
    return completedCheckpointIds.find(checkpointId) != completedCheckpointIds.end();
}

float Player::getProgress() const {
    if (totalCheckpoints == 0) return 0.0f;
    return static_cast<float>(checkpointsCompleted) / totalCheckpoints;
}
