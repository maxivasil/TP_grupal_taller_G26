#include "NPCTraffic.h"
#include "../../common/CarStats.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>

NPCTraffic::NPCTraffic(b2WorldId world, uint8_t carType_, b2Vec2 position):
        carType(carType_), car(nullptr), currentDirection(rand() % 4), stuckTimer(0.0f),
        collisionCount(0), isParked(false), lastPosition(position), 
        directionChangeTimer(0.0f), currentMovementDir(Direction::FORWARD) {
    // Obtener stats válidos según el tipo de auto
    std::vector<std::string> carNames = CarStatsDatabase::getAllCarNames();
    std::string selectedCar = carNames[carType_ % carNames.size()];
    CarStats stats = CarStatsDatabase::getCarStats(selectedCar);
    car = new NPCCar(world, stats, position, b2MakeRot(0));
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
            car->updateNPCPhysics(0.0f, Direction::FORWARD);  // Parado
        }
        return;
    }
    
    if (!car) return;
    
    // ===== VELOCIDAD CONSTANTE Y SIMPLE =====
    // Todos los NPCs avanzan a 0.5 (50% de velocidad máxima)
    float cruiseSpeed = 0.5f;
    
    // ===== DECIDIR SI GIRAR O CONTINUAR RECTO =====
    directionChangeTimer += deltaTime;
    
    // Cada 4-8 segundos, decidir si girar o continuar
    if (directionChangeTimer >= MIN_TIME_BETWEEN_TURNS) {
        // 80% probabilidad de continuar recto, 20% de girar
        int randomChoice = rand() % 100;
        
        if (randomChoice < 20) {
            // Girar: 50% izquierda, 50% derecha
            currentMovementDir = (rand() % 2 == 0) ? Direction::LEFT : Direction::RIGHT;
        } else {
            // Continuar recto
            currentMovementDir = Direction::FORWARD;
        }
        
        // Resetear timer con variación (4-8 segundos)
        directionChangeTimer = 0.0f;
        MIN_TIME_BETWEEN_TURNS = 4.0f + (rand() % 40) / 10.0f;
    }
    
    // ===== APLICAR MOVIMIENTO SIMPLE =====
    // Siempre avanzar a velocidad constante, cambiar dirección según timer o colisión
    car->updateNPCPhysics(cruiseSpeed, currentMovementDir);
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

    // Rotar instantáneamente al colisionar
    // Elegir dirección aleatoria (izquierda o derecha) para evitar obstáculo
    Direction evasionDirection = (rand() % 2 == 0) ? Direction::LEFT : Direction::RIGHT;
    
    if (car) {
        NPCCar* npcCar = dynamic_cast<NPCCar*>(car);
        if (npcCar) {
            npcCar->rotateBodyToDirection(evasionDirection);
        }
    }
    
    // Actualizar el estado interno también
    currentMovementDir = evasionDirection;
    totalCollisionsRecent = 0;  // Reset después de aplicar rotación
    collisionResetTimer = 0.0f;
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

