#pragma once

#include "CheckpointSpawner.h"
#include "../NPCTraffic.h"
#include <memory>
#include <unordered_map>



/**
 * @class NPCCheckpointManager
 * @brief Gestor de NPCs distribuidos en checkpoints
 * 
 * Integra CheckpointSpawner con NPCTraffic/NPCCar para:
 * - Spawnear NPCs automáticamente en checkpoints
 * - Despawnear cuando se alejan
 * - Mantener densidad equilibrada
 * - Minimizar choques mediante distribución inteligente
 */
class NPCCheckpointManager {
private:
    std::shared_ptr<CheckpointSpawner> spawner;
    
    // Mapping: NPC ID -> Checkpoint ID asignado
    std::unordered_map<int, int> npcToCheckpoint;
    
    // Parámetros
    int targetNPCCount = 15;
    int maxNPCsPerCheckpoint = 4;
    float despawnDistance = 200.0f;
    
public:
    explicit NPCCheckpointManager(std::shared_ptr<CheckpointSpawner> checkpointSpawner);
    virtual ~NPCCheckpointManager() = default;
    
    // === CONFIGURACIÓN ===
    
    void setTargetNPCCount(int count);
    void setMaxNPCsPerCheckpoint(int max);
    void setDespawnDistance(float distance);
    
    // === SPAWNING ===
    
    /**
     * @brief Intenta spawnear un nuevo NPC
     * @return true si se spawneó exitosamente
     */
    bool trySpawnNPC(int& outNPCId, b2Vec2& outPosition);
    
    /**
     * @brief Registra un NPC como spawneado
     */
    void registerNPC(int npcId, int checkpointId);
    
    /**
     * @brief Desregistra un NPC
     */
    void unregisterNPC(int npcId);
    
    // === QUERIES ===
    
    int getCurrentNPCCount() const;
    const Checkpoint* getCheckpointForNPC(int npcId) const;
    const std::vector<Checkpoint>& getAllCheckpoints() const;
    
    // === DEBUG ===
    
    void printStatistics() const;
};
