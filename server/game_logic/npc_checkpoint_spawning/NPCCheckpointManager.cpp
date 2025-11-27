#include "NPCCheckpointManager.h"
#include <iostream>

NPCCheckpointManager::NPCCheckpointManager(std::shared_ptr<CheckpointSpawner> checkpointSpawner)
    : spawner(checkpointSpawner) {
    if (spawner) {
        spawner->setTotalNPCCount(targetNPCCount);
    }
}

void NPCCheckpointManager::setTargetNPCCount(int count) {
    targetNPCCount = count;
    if (spawner) {
        spawner->setTotalNPCCount(count);
    }
}

void NPCCheckpointManager::setMaxNPCsPerCheckpoint(int max) {
    maxNPCsPerCheckpoint = max;
}

void NPCCheckpointManager::setDespawnDistance(float distance) {
    despawnDistance = distance;
}

bool NPCCheckpointManager::trySpawnNPC(int& outNPCId, b2Vec2& outPosition) {
    if (!spawner || getCurrentNPCCount() >= targetNPCCount) {
        return false;
    }
    
    // Obtener checkpoint con espacio disponible
    auto availableCheckpoints = spawner->getAvailableCheckpoints(maxNPCsPerCheckpoint);
    if (availableCheckpoints.empty()) {
        return false;
    }
    
    // Seleccionar checkpoint con menos NPCs
    const Checkpoint* selected = availableCheckpoints[0];
    for (const auto* cp : availableCheckpoints) {
        if (spawner->getNPCCountAtCheckpoint(cp->checkpointId) < 
            spawner->getNPCCountAtCheckpoint(selected->checkpointId)) {
            selected = cp;
        }
    }
    
    // Calcular posición de spawn
    outPosition = spawner->calculateSpawnPosition(selected);
    outNPCId = selected->checkpointId * 1000 + npcToCheckpoint.size();  // ID único
    
    return true;
}

void NPCCheckpointManager::registerNPC(int npcId, int checkpointId) {
    npcToCheckpoint[npcId] = checkpointId;
    if (spawner) {
        spawner->registerNPCAtCheckpoint(checkpointId);
    }
}

void NPCCheckpointManager::unregisterNPC(int npcId) {
    auto it = npcToCheckpoint.find(npcId);
    if (it != npcToCheckpoint.end()) {
        if (spawner) {
            spawner->unregisterNPCAtCheckpoint(it->second);
        }
        npcToCheckpoint.erase(it);
    }
}

int NPCCheckpointManager::getCurrentNPCCount() const {
    return npcToCheckpoint.size();
}

const Checkpoint* NPCCheckpointManager::getCheckpointForNPC(int npcId) const {
    auto it = npcToCheckpoint.find(npcId);
    if (it != npcToCheckpoint.end() && spawner) {
        return spawner->getCheckpointById(it->second);
    }
    return nullptr;
}

const std::vector<Checkpoint>& NPCCheckpointManager::getAllCheckpoints() const {
    static const std::vector<Checkpoint> empty;
    if (spawner) {
        return spawner->getCheckpoints();
    }
    return empty;
}

void NPCCheckpointManager::printStatistics() const {
    std::cout << "\n=== NPC Checkpoint Manager Statistics ===" << std::endl;
    std::cout << "Target NPCs: " << targetNPCCount << std::endl;
    std::cout << "Current NPCs: " << getCurrentNPCCount() << std::endl;
    std::cout << "Max per checkpoint: " << maxNPCsPerCheckpoint << std::endl;
    
    if (spawner) {
        std::cout << "Total checkpoints: " << spawner->getCheckpointCount() << std::endl;
        
        for (const auto& cp : spawner->getCheckpoints()) {
            int count = spawner->getNPCCountAtCheckpoint(cp.checkpointId);
            std::cout << "  Checkpoint " << cp.checkpointId << ": " << count << " NPCs" << std::endl;
        }
    }
}
