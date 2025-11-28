#include "NPCTraffic.h"
#include "../../common/CarStats.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>

NPCTraffic::NPCTraffic(b2WorldId world, uint8_t carType_, b2Vec2 position):
        carType(carType_), car(nullptr), currentDirection(0), stuckTimer(0.0f),
        collisionCount(0), isParked(false), lastPosition(position), 
        directionChangeTimer(0.0f), currentMovementDir(Direction::FORWARD) {
    // Obtener stats válidos según el tipo de auto
    std::vector<std::string> carNames = CarStatsDatabase::getAllCarNames();
    std::string selectedCar = carNames[carType_ % carNames.size()];
    CarStats stats = CarStatsDatabase::getCarStats(selectedCar);
    
    // Spawnear mirando hacia el Norte (-Y) para mejor posicionamiento inicial
    car = new NPCCar(world, stats, position, b2MakeRot(-1.5708f));  // -π/2 radianes
}

NPCTraffic::~NPCTraffic() {
    if (car) delete car;
}

void NPCTraffic::updatePhysics(float deltaTime, const void* route) {
    if (isDestroyed()) {
        return;
    }
    
    // Si está estacionado, no moverse
    if (isParked) {
        if (car) {
            car->updateNPCPhysics(0.0f, Direction::FORWARD);
        }
        return;
    }
    
    if (!car) return;

    // Obtener posición actual
    b2Vec2 currentPos = car->getPosition();
    
    // Velocidad base (metros/segundo)
    float targetSpeed = 0.5f;
    
    // ===== FASE DE RETROCESO =====
    if (isReversing) {
        reverseTimer += deltaTime;
        
        if (reverseTimer < 1.0f) {
            // Retroceder durante 1 segundo para asegurar que se libra del obstáculo
            car->updateNPCPhysics(-0.5f, Direction::FORWARD);
        } else {
            // Terminó el retroceso, parar y cambiar dirección
            isReversing = false;
            isRotating = true;
            reverseTimer = 0.0f;
            
            // Parar completamente
            car->updateNPCPhysics(0.0f, Direction::FORWARD);
            
            // Cambiar a una dirección aleatoria
            if ((rand() % 2) == 0) {
                currentDirection = (currentDirection + 1) % 4;
            } else {
                currentDirection = (currentDirection + 3) % 4;
            }
        }
        
        lastPosition = currentPos;
        return;
    }

    // ===== FASE DE ROTACIÓN =====
    if (isRotating) {
        float targetAngle = 0.0f;
        switch (currentDirection % 4) {
            case 0: targetAngle = -1.5708f; break;  // -π/2
            case 1: targetAngle = 0.0f; break;
            case 2: targetAngle = 1.5708f; break;   // π/2
            case 3: targetAngle = 3.14159f; break;  // π
        }
        
        b2Rot currentRot = car->getRotation();
        float currentAngle = b2Rot_GetAngle(currentRot);
        
        // Normalizar ángulos
        while (targetAngle > 3.14159f) targetAngle -= 6.28318f;
        while (targetAngle < -3.14159f) targetAngle += 6.28318f;
        while (currentAngle > 3.14159f) currentAngle -= 6.28318f;
        while (currentAngle < -3.14159f) currentAngle += 6.28318f;
        
        float angleDiff = std::abs(targetAngle - currentAngle);
        if (angleDiff > 3.14159f) {
            angleDiff = 6.28318f - angleDiff;
        }
        
        if (angleDiff < 0.15f) {
            // Rotación completada
            isRotating = false;
            lastRotatedDirection = currentDirection;
            lastPosition = currentPos;  // Resetear posición para nuevo movimiento
            stuckTimer = 0.0f;  // Resetear stuck timer
            // Continuar al código de movimiento normal (no retornar)
        } else {
            // Aún rotando
            NPCCar* npcCar = dynamic_cast<NPCCar*>(car);
            if (npcCar) {
                npcCar->rotateToAngle(targetAngle);
            }
            lastPosition = currentPos;
            return;
        }
    }

    // ===== DETECCIÓN DE ATASCAMIENTO Y MOVIMIENTO NORMAL =====
    float distMoved = b2Distance(lastPosition, currentPos);
    stuckTimer += deltaTime;

    // Si el NPC se movió muy poco en este frame Y se intenta mover
    if (distMoved < 0.05f && stuckTimer > 0.3f) {
        // Hay un obstáculo, iniciar retroceso
        isReversing = true;
        reverseTimer = 0.0f;
        car->updateNPCPhysics(0.0f, Direction::FORWARD);
        lastPosition = currentPos;
        return;
    }

    // Resetear stuck timer si se está moviendo bien
    if (distMoved > 0.1f) {
        stuckTimer = 0.0f;
    }

    // Recordar posición para próximo frame
    lastPosition = currentPos;

    // Vector de dirección según currentDirection
    b2Vec2 direction = {0.0f, 0.0f};
    float targetAngle = 0.0f;

    switch (currentDirection % 4) {
        case 0:  // Norte (arriba, -Y)
            direction = {0.0f, -1.0f};
            targetAngle = -1.5708f;
            break;
        case 1:  // Este (derecha, +X)
            direction = {1.0f, 0.0f};
            targetAngle = 0.0f;
            break;
        case 2:  // Sur (abajo, +Y)
            direction = {0.0f, 1.0f};
            targetAngle = 1.5708f;
            break;
        case 3:  // Oeste (izquierda, -X)
            direction = {-1.0f, 0.0f};
            targetAngle = 3.14159f;
            break;
    }

    // Aplicar velocidad
    car->updateNPCPhysics(targetSpeed, Direction::FORWARD);
    
    // ROTACIÓN SOLO SI CAMBIÓ LA DIRECCIÓN
    if (currentDirection != lastRotatedDirection) {
        NPCCar* npcCar = dynamic_cast<NPCCar*>(car);
        if (npcCar) {
            npcCar->rotateToAngle(targetAngle);
            lastRotatedDirection = currentDirection;
        }
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

