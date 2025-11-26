#include "NPCTraffic.h"
#include "../../common/CarStats.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>

NPCTraffic::NPCTraffic(b2WorldId world, uint8_t carType_, b2Vec2 position):
        carType(carType_), car(nullptr), currentDirection(rand() % 4), stuckTimer(0.0f),
        collisionCount(0), isParked(false) {
    // Obtener stats válidos según el tipo de auto
    std::vector<std::string> carNames = CarStatsDatabase::getAllCarNames();
    std::string selectedCar = carNames[carType_ % carNames.size()];
    CarStats stats = CarStatsDatabase::getCarStats(selectedCar);
    car = new Car(world, stats, position, b2MakeRot(0));
}

NPCTraffic::~NPCTraffic() {
    if (car) delete car;
}

void NPCTraffic::updatePhysics(float deltaTime, const std::vector<RoutePoint>* route) {
    if (isDestroyed()) {
        return;
    }
    // Si está estacionado, no moverse
    if (isParked) {
        if (car) {
            car->updatePhysics(CarInput{false, true, Direction::FORWARD});
        }
        return;
    }
    // Movimiento NPC: delegar a Car
    if (car) {
        car->updatePhysics(CarInput{true, false, Direction::FORWARD});
    }
}

b2Vec2 NPCTraffic::getPosition() const {
    return car ? car->getPosition() : b2Vec2{0, 0};
}

b2Rot NPCTraffic::getRotation() const {
    return car ? car->getRotation() : b2Rot();
}

b2Vec2 NPCTraffic::getLinearVelocity() const {
    return car ? car->getLinearVelocity() : b2Vec2{0, 0};
}

void NPCTraffic::onCollision(Collidable* other, float approachSpeed, float deltaTime,
                             const b2Vec2& contactNormal) {
    float damage = std::max(0.0f, (approachSpeed - 2.0f) * 10.0f);
    takeDamage(damage);

    // ===== SISTEMA DE CONTEO DE COLISIONES =====
    // Incrementar contador total de colisiones recientes
    totalCollisionsRecent++;
    collisionResetTimer = 0.0f;  // Resetear timer cuando hay colisión

    // Si hay colisiones frecuentes, DETENER al auto y cambiar dirección
    if (totalCollisionsRecent > 3) {
        // Detener completamente usando la clase Car
        if (car) {
            car->updatePhysics(CarInput{false, true, Direction::FORWARD});
        }
        
        // Cambiar dirección aleatoriamente
        currentDirection = rand() % 4;
        totalCollisionsRecent = 0;
        
        std::cout << "[NPC] Demasiadas colisiones, cambiando dirección" << std::endl;
    }

    // Si supera el máximo, marcar para respawn
    if (totalCollisionsRecent > MAX_COLLISIONS_BEFORE_RESPAWN) {
        // El NPC necesita respawn - la carrera lo detectará y lo hará
        std::cout << "[NPC] NPC en posición (" << getPosition().x << ", " 
                  << getPosition().y << ") requiere respawn (colisiones: " 
                  << totalCollisionsRecent << ")" << std::endl;
        totalCollisionsRecent = 0;  // Resetear contador
    }
}

b2Rot NPCTraffic::getRotation(const b2Vec2& contactNormal) const {
    return getRotation();
}

void NPCTraffic::applyCollision(const CollisionInfo& info) {
    if (car) car->applyCollision(info);
}

void NPCTraffic::resetPosition(b2Vec2 newPos) {
    if (car) {
        // Si Car tiene método para teletransportar, usarlo. Si no, se puede reinicializar la posición.
        // car->setPosition(newPos); // Si existe
        car->repair(); // O simplemente reparar y dejar en nueva posición si se implementa
    }
    stuckTimer = 0.0f;
    collisionCount = 0;
    totalCollisionsRecent = 0;
    collisionResetTimer = 0.0f;
    currentDirection = rand() % 4;
    std::cout << "[NPC] Respawn en posición (" << newPos.x << ", " << newPos.y << ")" << std::endl;
}

