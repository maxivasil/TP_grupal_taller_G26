#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <box2d/box2d.h>

/**
 * @struct Checkpoint
 * @brief Punto de referencia en el track donde se pueden spawnear NPCs
 */
struct Checkpoint {
    int checkpointId;
    b2Vec2 position;
    float radius;  // Radio de spawning alrededor del checkpoint
    
    Checkpoint(int id, b2Vec2 pos, float r = 15.0f)
        : checkpointId(id), position(pos), radius(r) {}
};

/**
 * @class CheckpointSpawner
 * @brief Sistema de distribución de NPCs basado en checkpoints del track
 * 
 * Características:
 * - Lee checkpoints de un track YAML
 * - Distribuye NPCs uniformemente entre checkpoints
 * - Evita concentración en un solo checkpoint
 * - Ajusta dinámicamente según densidad de tráfico
 */
class CheckpointSpawner {
private:
    std::vector<Checkpoint> checkpoints;
    std::unordered_map<int, int> checkpointVehicleCount;  // ID checkpoint -> cantidad de NPCs
    
    int totalNPCsToSpawn = 0;
    int currentNPCCount = 0;
    int nextCheckpointIndex = 0;  // Round-robin para distribución
    
public:
    CheckpointSpawner() = default;
    virtual ~CheckpointSpawner() = default;
    
    // === CONFIGURACIÓN ===
    
    /**
     * @brief Agrega un checkpoint al sistema
     */
    void addCheckpoint(int id, b2Vec2 position, float radius = 15.0f);
    
    /**
     * @brief Carga checkpoints desde archivo YAML
     * @param yamlPath Ruta al archivo (ej: "tracks/vice_city.yaml")
     */
    bool loadCheckpointsFromYAML(const std::string& yamlPath);
    
    /**
     * @brief Establece el total de NPCs a spawnear
     */
    void setTotalNPCCount(int count);
    
    // === SPAWNING ===
    
    /**
     * @brief Obtiene el próximo checkpoint donde spawnear un NPC
     * Usa distribución round-robin para equilibrio
     * 
     * @return Checkpoint para spawn, o nullptr si no hay checkpoints
     */
    const Checkpoint* getNextSpawnCheckpoint();
    
    /**
     * @brief Obtiene checkpoint específico por ID
     */
    const Checkpoint* getCheckpointById(int id) const;
    
    /**
     * @brief Obtiene checkpoint más cercano a una posición
     */
    const Checkpoint* getNearestCheckpoint(b2Vec2 position) const;
    
    /**
     * @brief Calcula posición de spawn dentro del checkpoint
     * (con pequeño offset aleatorio para evitar aglomeración)
     */
    b2Vec2 calculateSpawnPosition(const Checkpoint* checkpoint);
    
    // === ESTADÍSTICAS ===
    
    /**
     * @brief Retorna cantidad de NPCs asignados a un checkpoint
     */
    int getNPCCountAtCheckpoint(int checkpointId) const;
    
    /**
     * @brief Incrementa contador de NPCs en checkpoint
     */
    void registerNPCAtCheckpoint(int checkpointId);
    
    /**
     * @brief Decrementa contador de NPCs en checkpoint
     */
    void unregisterNPCAtCheckpoint(int checkpointId);
    
    // === GETTERS ===
    
    const std::vector<Checkpoint>& getCheckpoints() const { return checkpoints; }
    int getCheckpointCount() const { return checkpoints.size(); }
    int getTargetNPCCount() const { return totalNPCsToSpawn; }
    int getCurrentNPCCount() const { return currentNPCCount; }
    
    // === QUERIES ===
    
    /**
     * @brief Verifica si hay espacio para más NPCs en un checkpoint
     * (basado en límite de densidad)
     */
    bool hasSpaceAtCheckpoint(int checkpointId, int maxPerCheckpoint = 5) const;
    
    /**
     * @brief Retorna todos los checkpoints con espacio disponible
     */
    std::vector<const Checkpoint*> getAvailableCheckpoints(int maxPerCheckpoint = 5) const;
};
