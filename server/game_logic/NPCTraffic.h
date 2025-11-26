#ifndef NPC_TRAFFIC_H
#define NPC_TRAFFIC_H

#include <cmath>
#include <vector>

#include <box2d/box2d.h>

#include "Collidable.h"
#include "npc_routes_from_checkpoints.h"

/**
 * @class NPCTraffic
 * @brief Representa un auto de tráfico (NPC) con física y colisiones
 *
 * Los NPCs son autos controlados por IA que circulan por rutas predefinidas.
 * Tienen cuerpos físicos Box2D y pueden colisionar con otros vehículos.
 */
class NPCTraffic: public Collidable {
private:
    b2BodyId body;
    uint8_t carType;       // Tipo de auto (0-6)
    float current_health;  // Vida actual
    float maxHealth;       // Vida máxima

    // Datos de ruta
    int currentRoutePoint = 0;   // Índice del waypoint actual
    float routeProgress = 0.0f;  // Progreso hacia el siguiente waypoint (0-1)
    float targetSpeed = 3.0f;    // Velocidad objetivo según la ruta
    const std::vector<RoutePoint>* assignedRoute = nullptr;  // Ruta asignada
    int routeIndex = -1;  // Índice de la ruta en la lista de rutas (para acceso dinámico)

    // Movimiento aleatorio inteligente (basado en esquinas/colisiones)
    int currentDirection = 0;  // 0=N, 1=E, 2=S, 3=O
    b2Vec2 lastPosition;       // Posición anterior para detectar si se movió
    float stuckTimer = 0.0f;   // Timer para detectar si está estancado
    int collisionCount = 0;    // Contador de colisiones recientes

    // Sistema de respawn
    int totalCollisionsRecent = 0;     // Total de colisiones recientes
    float collisionResetTimer = 0.0f;  // Timer para resetear contador de colisiones
    static constexpr int MAX_COLLISIONS_BEFORE_RESPAWN =
            10;  // Máximo de colisiones antes de respawn
    static constexpr float COLLISION_RESET_TIME =
            5.0f;  // Resetear contador cada 5 segundos sin colisiones

    // Estado del auto
    bool isParked = false;  // Si el auto está estacionado

    // Masas típicas para autos (kg)
    static constexpr float CAR_MASS = 1000.0f;

    b2BodyDef initNPCBodyDef(b2Vec2 position);
    void setShape(b2BodyId body);

public:
    NPCTraffic(b2WorldId world, uint8_t carType, b2Vec2 position);
    ~NPCTraffic() override;

    /**
     * @brief Asigna una ruta al NPC
     * @param route Puntos de la ruta a seguir
     */
    void setRoute(const std::vector<RoutePoint>* route) { assignedRoute = route; }

    /**
     * @brief Establece el índice de ruta para acceso dinámico
     * @param idx Índice de la ruta en currentRoutes (para Race::updatePhysics)
     */
    void setRouteIndex(int idx) { routeIndex = idx; }

    /**
     * @brief Establece si el auto está estacionado (sin movimiento)
     */
    void setParked(bool parked) { isParked = parked; }

    /**
     * @brief Obtiene si el auto está estacionado
     */
    bool getParked() const { return isParked; }

    /**
     * @brief Resetea la posición del NPC a una nueva ubicación
     */
    void resetPosition(b2Vec2 newPos);

    /**
     * @brief Obtiene el contador de colisiones recientes
     */
    int getTotalCollisionsRecent() const { return totalCollisionsRecent; }

    /**
     * @brief Actualiza la posición del NPC siguiendo una ruta
     * @param deltaTime Tiempo transcurrido en segundos
     * @param route Puntos de la ruta a seguir (puede ser nullptr si ya hay ruta asignada)
     */
    void updatePhysics(float deltaTime, const std::vector<RoutePoint>* route);

    /**
     * @brief Obtiene la posición actual del NPC
     */
    b2Vec2 getPosition() const;

    /**
     * @brief Obtiene la rotación actual del NPC
     */
    b2Rot getRotation() const;

    /**
     * @brief Obtiene la velocidad lineal del NPC
     */
    b2Vec2 getLinearVelocity() const;

    /**
     * @brief Obtiene el tipo de auto
     */
    uint8_t getCarType() const { return carType; }

    /**
     * @brief Obtiene la salud actual del NPC
     */
    float getCurrentHealth() const { return current_health; }

    /**
     * @brief Verifica si el NPC fue destruido
     */
    bool isDestroyed() const { return current_health <= 0.0f; }

    /**
     * @brief Aplica daño al NPC
     */
    void takeDamage(float damage) { current_health -= damage; }

    /**
     * @brief Repara el NPC a salud máxima
     */
    void repair() { current_health = maxHealth; }

    // Métodos de Collidable
    void onCollision(Collidable* other, float approachSpeed, float deltaTime,
                     const b2Vec2& contactNormal) override;

    float getMass() const override { return CAR_MASS; }

    b2Rot getRotation(const b2Vec2& contactNormal) const override;

    void applyCollision(const CollisionInfo& info) override;
};

#endif  // NPC_TRAFFIC_H
