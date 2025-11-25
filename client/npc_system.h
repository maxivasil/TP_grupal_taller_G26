#ifndef NPC_SYSTEM_H
#define NPC_SYSTEM_H

#include <cmath>
#include <cstdint>
#include <random>
#include <utility>
#include <vector>

#include "npc_routes.h"  // Incluir para RoutePoint

/**
 * @struct NPC
 * @brief Representa un auto de tráfico circulando por la ciudad (no jugador)
 */
struct NPC {
    // Posición en coordenadas del servidor (metros)
    float pos_x = 0.0f;
    float pos_y = 0.0f;

    // Tipo de auto (0-6, correspondiente a los 7 autos disponibles)
    uint8_t car_type = 0;

    // Dirección de movimiento actual (0-3: N, E, S, O) - OBSOLETO en modo ruta
    uint8_t direction = 0;

    // Orientación en grados (0-360)
    float angle = 0.0f;

    // Velocidad en metros/segundo
    float speed = 3.0f;  // Velocidad base de NPCs

    // Ruta predeterminada
    int routeId = -1;            // ID de la ruta que sigue (-1 = sin ruta)
    int currentRoutePoint = 0;   // Índice del punto actual en la ruta
    float routeProgress = 0.0f;  // Progreso hacia el siguiente punto (0.0-1.0)

    // Tiempo hasta el próximo cambio de dirección (en segundos) - OBSOLETO en modo ruta
    float timeTillDirectionChange = 2.0f;
    float timeTillDirectionChangeCounter = 0.0f;

    // Si el NPC está detenido (estacionado)
    bool isParked = false;

    // Contador de frames para animar estacionamiento
    int parkedFrames = 0;

    // Constructor
    NPC() = default;

    NPC(float x, float y, uint8_t type):
            pos_x(x),
            pos_y(y),
            car_type(type),
            direction(rand() % 4),
            angle(0.0f),
            speed(2.0f + (rand() % 3)) {}

    /**
     * @brief Actualiza la posición del NPC según su ruta o movimiento aleatorio
     * @param deltaTime Tiempo transcurrido en segundos
     * @param routePoints Puntos de la ruta actual (nullptr = movimiento aleatorio)
     */
    void update(float deltaTime, const std::vector<RoutePoint>* routePoints = nullptr) {
        // Si tiene ruta asignada, seguirla
        if (routeId != -1 && routePoints != nullptr && !routePoints->empty()) {
            followRoute(deltaTime, routePoints);
        } else {
            // Fallback: movimiento aleatorio
            updateRandom(deltaTime);
        }
    }

    /**
     * @brief Sigue una ruta predeterminada
     */
    void followRoute(float deltaTime, const std::vector<RoutePoint>* routePoints) {
        if (routePoints->empty())
            return;

        // Obtener punto actual y siguiente
        size_t currentIdx = currentRoutePoint;
        size_t nextIdx = (currentIdx + 1) % routePoints->size();

        const RoutePoint& current = (*routePoints)[currentIdx];
        const RoutePoint& next = (*routePoints)[nextIdx];

        // Velocidad del segmento actual
        speed = current.speed;

        // Vector desde posición actual al siguiente waypoint
        float dx = next.x - pos_x;
        float dy = next.y - pos_y;
        float distance = std::sqrt(dx * dx + dy * dy);

        // Si llegamos al waypoint, avanzar al siguiente
        if (distance < 0.5f) {
            currentRoutePoint = (currentRoutePoint + 1) % routePoints->size();
            return;
        }

        // Normalizar dirección
        dx /= distance;
        dy /= distance;

        // Actualizar ángulo (atan2 retorna radianes)
        angle = std::atan2(dy, dx) * 180.0f / 3.14159265f;
        if (angle < 0.0f)
            angle += 360.0f;

        // Mover hacia el waypoint
        float moveDistance = speed * deltaTime;
        pos_x += dx * moveDistance;
        pos_y += dy * moveDistance;

        isParked = false;
    }

    /**
     * @brief Movimiento aleatorio (fallback si no hay ruta)
     */
    void updateRandom(float deltaTime) {
        if (isParked) {
            parkedFrames++;
            // 10% de probabilidad de dejar de estar estacionado cada segundo
            if (parkedFrames > 30 && (rand() % 100) < 5) {
                isParked = false;
                parkedFrames = 0;
                direction = rand() % 4;
            }
            return;
        }

        // Actualizar contador de cambio de dirección
        timeTillDirectionChangeCounter += deltaTime;

        // Cambiar dirección aleatoriamente
        if (timeTillDirectionChangeCounter >= timeTillDirectionChange) {
            timeTillDirectionChangeCounter = 0.0f;
            // 40% de probabilidad de cambiar dirección
            if ((rand() % 100) < 40) {
                direction = rand() % 4;
            }
            // 15% de probabilidad de detenerse (estacionarse)
            if ((rand() % 100) < 15) {
                isParked = true;
                parkedFrames = 0;
            }
            timeTillDirectionChange = 0.5f + (rand() % 2);  // 0.5-2.5 segundos
        }

        // Calcular velocidad en x,y según dirección
        float vx = 0.0f, vy = 0.0f;

        switch (direction) {
            case 0:  // Norte (arriba) - ángulo 270°
                vy = -speed;
                angle = 270.0f;
                break;
            case 1:  // Este (derecha) - ángulo 0°
                vx = speed;
                angle = 0.0f;
                break;
            case 2:  // Sur (abajo) - ángulo 90°
                vy = speed;
                angle = 90.0f;
                break;
            case 3:  // Oeste (izquierda) - ángulo 180°
                vx = -speed;
                angle = 180.0f;
                break;
        }

        // Actualizar posición
        pos_x += vx * deltaTime;
        pos_y += vy * deltaTime;
    }

    /**
     * @brief Verifica si el NPC está dentro de los límites del mapa
     * @param minX, maxX, minY, maxY Límites del mundo
     * @return true si está dentro, false si necesita "teletransportarse"
     */
    bool isInBounds(float minX, float maxX, float minY, float maxY) const {
        return pos_x >= minX && pos_x <= maxX && pos_y >= minY && pos_y <= maxY;
    }

    /**
     * @brief Teletransporta el NPC al lado opuesto del mapa
     */
    void wrapAround(float minX, float maxX, float minY, float maxY) {
        if (pos_x < minX)
            pos_x = maxX;
        else if (pos_x > maxX)
            pos_x = minX;

        if (pos_y < minY)
            pos_y = maxY;
        else if (pos_y > maxY)
            pos_y = minY;
    }

    /**
     * @brief Calcula el radio de colisión del NPC para detección de colisiones
     * @return Radio aproximado del auto en metros (basado en tipo)
     */
    float getCollisionRadius() const {
        // Aproximadamente la mitad del tamaño más pequeño del auto
        // Los autos son ~0.28m de ancho en el mundo
        return 0.25f;
    }

    /**
     * @brief Verifica si este NPC colisiona con una posición dada
     * @param otherX, otherY Posición a verificar
     * @param otherRadius Radio de colisión del otro objeto
     * @return true si hay colisión
     */
    bool checkCollision(float otherX, float otherY, float otherRadius) const {
        float dx = pos_x - otherX;
        float dy = pos_y - otherY;
        float distance = std::sqrt(dx * dx + dy * dy);
        return distance < (getCollisionRadius() + otherRadius);
    }
};

/**
 * @class NPCManager
 * @brief Gestor de todos los NPCs en el juego
 */
class NPCManager {
private:
    std::vector<NPC> npcs;
    std::mt19937 rng;

    // Límites del mundo (aproximadamente, según la ciudad)
    float worldMinX = 0.0f;
    float worldMaxX = 100.0f;
    float worldMinY = 0.0f;
    float worldMaxY = 100.0f;

    int maxNPCs = 30;  // Máximo número de NPCs simultáneamente

    // Configuración de distancia mínima entre NPCs (NIVEL 2)
    float minSpawnDistance = 80.0f;  // Distancia mínima en metros

    /**
     * @brief Verifica si una posición es válida para spawnear un NPC
     * @param x, y Coordenadas a verificar
     * @param minDistance Distancia mínima requerida a otros NPCs
     * @return true si es válida (sin NPCs cercanos), false si está muy cercano
     */
    bool isValidSpawnPosition(float x, float y, float minDistance) const {
        bool estado = std::none_of(npcs.begin(), npcs.end(), [&](const auto& npc) {
            float dx = npc.pos_x - x;
            float dy = npc.pos_y - y;
            float distance = std::sqrt(dx * dx + dy * dy);
            return distance < minDistance;
        });

        return estado;  // Posición válida
    }

public:
    NPCManager(): rng(std::random_device{}()) {}

    /**
     * @brief Genera NPCs iniciales distribuidos aleatoriamente en la ciudad
     */
    void initialize(int count = 10, float mapX = 100.0f, float mapY = 100.0f) {
        // Este método genera NPCs con límites por defecto
        // Luego debe llamarse setWorldBounds para actualizar los límites reales
        npcs.clear();
        worldMaxX = worldMinX + mapX;
        worldMaxY = worldMinY + mapY;
        maxNPCs = count;

        std::uniform_real_distribution<float> distX(worldMinX + 10.0f, worldMaxX - 10.0f);
        std::uniform_real_distribution<float> distY(worldMinY + 10.0f, worldMaxY - 10.0f);
        std::uniform_int_distribution<uint8_t> distType(0, 6);

        for (int i = 0; i < count; ++i) {
            float x = distX(rng);
            float y = distY(rng);
            uint8_t type = distType(rng);
            npcs.emplace_back(x, y, type);
        }
    }

    /**
     * @brief Regenera NPCs dentro de los nuevos límites especificados
     */
    void reinitializeWithBounds(int count, float minX, float maxX, float minY, float maxY) {
        npcs.clear();
        worldMinX = minX;
        worldMaxX = maxX;
        worldMinY = minY;
        worldMaxY = maxY;
        maxNPCs = count;

        std::uniform_real_distribution<float> distX(worldMinX + 10.0f, worldMaxX - 10.0f);
        std::uniform_real_distribution<float> distY(worldMinY + 10.0f, worldMaxY - 10.0f);
        std::uniform_int_distribution<uint8_t> distType(0, 6);

        for (int i = 0; i < count; ++i) {
            float x = distX(rng);
            float y = distY(rng);
            uint8_t type = distType(rng);
            npcs.emplace_back(x, y, type);
        }
    }

    /**
     * @brief Configura la distancia mínima entre NPCs en spawn points (NIVEL 2)
     * @param distance Nueva distancia mínima en píxeles (default: 80.0)
     */
    void setMinSpawnDistance(float distance) { minSpawnDistance = distance; }

    /**
     * @brief Obtiene la distancia mínima configurada
     * @return La distancia mínima entre NPCs al spawnear
     */
    float getMinSpawnDistance() const { return minSpawnDistance; }

    /**
     * @brief Genera NPCs desde spawn points definidos en las rutas
     * CON VALIDACIÓN DE DISTANCIA MÍNIMA (NIVEL 2)
     * @param routes Las rutas que contienen spawn points
     * @param minX, maxX, minY, maxY Límites del mundo
     */
    void initializeFromSpawnPoints(const std::vector<NPCRoute>& routes, float minX, float maxX,
                                   float minY, float maxY) {
        npcs.clear();
        worldMinX = minX;
        worldMaxX = maxX;
        worldMinY = minY;
        worldMaxY = maxY;

        // Colectar todos los spawn points de todas las rutas
        std::vector<std::pair<SpawnPoint, int>> allSpawnPoints;  // (spawn, route_index)
        for (size_t routeIdx = 0; routeIdx < routes.size(); ++routeIdx) {
            for (const auto& spawn: routes[routeIdx].spawn_points) {
                // cppcheck-suppress useStlAlgorithm
                allSpawnPoints.push_back({spawn, (int)routeIdx});
            }
        }

        // Crear NPCs en cada spawn point CON VALIDACIÓN (NIVEL 2)
        int rejected = 0;
        for (const auto& [spawn, routeIdx]: allSpawnPoints) {
            // NIVEL 2: Verificar distancia mínima antes de spawnear
            if (!isValidSpawnPosition(spawn.x, spawn.y, minSpawnDistance)) {
                rejected++;
                continue;  // Rechazar este spawn point, está muy cerca de otro
            }

            NPC npc(spawn.x, spawn.y, spawn.car_type);
            npc.angle = spawn.direction;
            npc.routeId = spawn.route_index >= 0 ? spawn.route_index : routeIdx;
            npcs.push_back(npc);
        }

        maxNPCs = npcs.size();

        // Debug info (opcional, descomenta si necesitas ver qué pasa)
        std::cout << "[NPC SPAWN] Spawn points procesados: " << allSpawnPoints.size()
                  << ", rechazados: " << rejected << ", NPCs creados: " << npcs.size()
                  << " (minDist: " << minSpawnDistance << ")" << std::endl;
    }

    /**
     * @brief Actualiza todos los NPCs
     */
    void update(float deltaTime) {
        for (auto& npc: npcs) {
            // Llamar al update sin rutas (movimiento aleatorio)
            // Las rutas se integrarán después en client_game.cpp
            npc.update(deltaTime, nullptr);

            // Hacer wrap-around cuando se salen del mapa
            if (!npc.isInBounds(worldMinX, worldMaxX, worldMinY, worldMaxY)) {
                npc.wrapAround(worldMinX, worldMaxX, worldMinY, worldMaxY);
            }
        }
    }

    /**
     * @brief Obtiene la lista de NPCs (const)
     */
    const std::vector<NPC>& getNPCs() const { return npcs; }

    /**
     * @brief Obtiene la lista de NPCs (mutable)
     */
    std::vector<NPC>& getNPCs() { return npcs; }

    /**
     * @brief Limpia todos los NPCs
     */
    void clear() { npcs.clear(); }

    /**
     * @brief Establece los límites del mundo
     */
    void setWorldBounds(float minX, float maxX, float minY, float maxY) {
        worldMinX = minX;
        worldMaxX = maxX;
        worldMinY = minY;
        worldMaxY = maxY;
    }

    /**
     * @brief Verifica colisión del jugador con NPCs
     * @param playerX, playerY Posición del jugador
     * @param playerRadius Radio de colisión del jugador (~0.25 metros)
     * @return vector de índices de NPCs que colisionan
     */
    std::vector<int> checkCollisionsWithPlayer(float playerX, float playerY, float playerRadius) {
        std::vector<int> collidingNPCs;
        for (size_t i = 0; i < npcs.size(); ++i) {
            if (npcs[i].checkCollision(playerX, playerY, playerRadius)) {
                collidingNPCs.push_back(i);
            }
        }
        return collidingNPCs;
    }

    /**
     * @brief Obtiene información de un NPC específico
     */
    const NPC* getNPC(int index) const {
        if (index >= 0 && index < static_cast<int>(npcs.size())) {
            return &npcs[index];
        }
        return nullptr;
    }
};

#endif  // NPC_SYSTEM_H
