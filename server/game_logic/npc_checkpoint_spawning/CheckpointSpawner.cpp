#include "CheckpointSpawner.h"
#include <yaml-cpp/yaml.h>
#include <algorithm>
#include <random>
#include <cmath>

void CheckpointSpawner::addCheckpoint(int id, b2Vec2 position, float radius) {
    checkpoints.emplace_back(id, position, radius);
    checkpointVehicleCount[id] = 0;
}

bool CheckpointSpawner::loadCheckpointsFromYAML(const std::string& yamlPath) {
    try {
        YAML::Node config = YAML::LoadFile(yamlPath);
        
        if (!config["checkpoints"]) {
            return false;
        }
        
        checkpoints.clear();
        checkpointVehicleCount.clear();
        
        for (const auto& cp : config["checkpoints"]) {
            int id = cp["id"].as<int>();
            float x = cp["x"].as<float>();
            float y = cp["y"].as<float>();
            float radius = cp["radius"].as<float>(15.0f);
            
            addCheckpoint(id, b2Vec2(x, y), radius);
        }
        
        return true;
    } catch (const std::exception& e) {
        // Error al cargar YAML
        return false;
    }
}

void CheckpointSpawner::setTotalNPCCount(int count) {
    totalNPCsToSpawn = count;
    currentNPCCount = 0;
    nextCheckpointIndex = 0;
}

const Checkpoint* CheckpointSpawner::getNextSpawnCheckpoint() {
    if (checkpoints.empty()) {
        return nullptr;
    }
    
    // Round-robin: usar siguiente checkpoint
    const Checkpoint* result = &checkpoints[nextCheckpointIndex];
    nextCheckpointIndex = (nextCheckpointIndex + 1) % checkpoints.size();
    
    return result;
}

const Checkpoint* CheckpointSpawner::getCheckpointById(int id) const {
    for (const auto& cp : checkpoints) {
        if (cp.checkpointId == id) {
            return &cp;
        }
    }
    return nullptr;
}

const Checkpoint* CheckpointSpawner::getNearestCheckpoint(b2Vec2 position) const {
    if (checkpoints.empty()) {
        return nullptr;
    }
    
    float minDist = FLT_MAX;
    const Checkpoint* nearest = nullptr;
    
    for (const auto& cp : checkpoints) {
        b2Vec2 diff = cp.position - position;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        
        if (dist < minDist) {
            minDist = dist;
            nearest = &cp;
        }
    }
    
    return nearest;
}

b2Vec2 CheckpointSpawner::calculateSpawnPosition(const Checkpoint* checkpoint) {
    if (!checkpoint) {
        return b2Vec2_zero;
    }
    
    // Offset aleatorio dentro del radio del checkpoint
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159265f);
    std::uniform_real_distribution<float> radiusDist(0.0f, checkpoint->radius);
    
    float angle = angleDist(rng);
    float radius = radiusDist(rng);
    
    b2Vec2 offset(radius * std::cos(angle), radius * std::sin(angle));
    return checkpoint->position + offset;
}

int CheckpointSpawner::getNPCCountAtCheckpoint(int checkpointId) const {
    auto it = checkpointVehicleCount.find(checkpointId);
    if (it != checkpointVehicleCount.end()) {
        return it->second;
    }
    return 0;
}

void CheckpointSpawner::registerNPCAtCheckpoint(int checkpointId) {
    checkpointVehicleCount[checkpointId]++;
    currentNPCCount++;
}

void CheckpointSpawner::unregisterNPCAtCheckpoint(int checkpointId) {
    if (checkpointVehicleCount[checkpointId] > 0) {
        checkpointVehicleCount[checkpointId]--;
        currentNPCCount--;
    }
}

bool CheckpointSpawner::hasSpaceAtCheckpoint(int checkpointId, int maxPerCheckpoint) const {
    int count = getNPCCountAtCheckpoint(checkpointId);
    return count < maxPerCheckpoint;
}

std::vector<const Checkpoint*> CheckpointSpawner::getAvailableCheckpoints(int maxPerCheckpoint) const {
    std::vector<const Checkpoint*> available;
    
    for (const auto& cp : checkpoints) {
        if (hasSpaceAtCheckpoint(cp.checkpointId, maxPerCheckpoint)) {
            available.push_back(&cp);
        }
    }
    
    return available;
}
