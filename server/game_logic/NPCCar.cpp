#include "NPCCar.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>

#define MINMOVEMENT 0.1f
#define MAXFRAMESBLOQUED 60
#define RETROCESO_FRAMES 90  // ~1.5 segundos a 60 FPS

NPCCar::NPCCar(b2WorldId world, const CarStats& stats, b2Vec2 position, b2Rot rotation, bool parked,
               uint8_t carType):
        Car(world, stats, position, rotation),
        isParked(parked),
        isBlocked(false),
        carType(carType) {}

CarInput NPCCar::generateRandomMovement() {
    // Dirección de giro: completamente aleatoria (33% cada una)
    int turnChoice = std::rand() % 3;  // 0: LEFT, 1: FORWARD, 2: RIGHT
    Direction turnDir = Direction::FORWARD;
    if (turnChoice == 0) {
        turnDir = Direction::LEFT;
    } else if (turnChoice == 2) {
        turnDir = Direction::RIGHT;
    }

    // Velocidad crucero: 5 unidades (moderada, no depende del auto)
    const float CRUISE_SPEED = 5.0f;
    b2Vec2 velocity = b2Body_GetLinearVelocity(body);
    float speed = b2Length(velocity);

    bool shouldAccelerate = (speed < CRUISE_SPEED * 0.95f);
    bool shouldBrake = (speed > CRUISE_SPEED * 1.05f);

    return {
        .accelerating = shouldAccelerate,
        .braking = shouldBrake,
        .turn_direction = turnDir
    };
}

void NPCCar::updatePhysics(const CarInput& input) {
    if (isInRetrocesoMode) {
        retrocesoFramesRemaining--;
        
        if (retrocesoFramesRemaining > RETROCESO_FRAMES / 2) {
            // Primera mitad: retroceder
            Car::updatePhysics({.accelerating = false, .braking = true, .turn_direction = Direction::FORWARD});
        } else if (retrocesoFramesRemaining > 0) {
            // Segunda mitad: frenar completamente
            Car::updatePhysics({.accelerating = false, .braking = true, .turn_direction = Direction::FORWARD});
            // Detener completamente
            b2Body_SetLinearVelocity(body, {0.0f, 0.0f});
        } else {
            // Terminó retroceso: cambiar a dirección aleatoria
            isInRetrocesoMode = false;
            
            // Seleccionar una dirección aleatoria (izquierda o derecha relativa a current heading)
            float currentHeading = b2Rot_GetAngle(b2Body_GetRotation(body));
            if (std::rand() % 2 == 0) {
                // Girar 90° a la izquierda
                retrocesoAngle = currentHeading + B2_PI / 2.0f;
            } else {
                // Girar 90° a la derecha
                retrocesoAngle = currentHeading - B2_PI / 2.0f;
            }
            b2Vec2 pos = b2Body_GetPosition(body);
            b2Body_SetTransform(body, pos, b2MakeRot(retrocesoAngle));
        }
        return;
    }
    
    // Generar movimiento aleatorio (dirección + velocidad)
    CarInput npcMovement = generateRandomMovement();
    Car::updatePhysics(npcMovement);
    
    if (!isParked) {
        handleBlocked();
    }
}

void NPCCar::handleBlocked() {
    b2Vec2 currentPos = b2Body_GetPosition(body);
    float distMoved = b2Distance(currentPos, lastPos);

    if (distMoved < MINMOVEMENT) {
        blockedFrames++;

        if (blockedFrames >= MAXFRAMESBLOQUED) {
            isBlocked = true;
        }
    } else {
        blockedFrames = 0;
        isBlocked = false;
    }

    lastPos = currentPos;
}

void NPCCar::chooseIntersectionDirection(int intersectionId) {
    std::vector<float> validAngles;

    if (intersectionId & DIR_RIGHT) {
        validAngles.push_back(0.0f);
    }
    if (intersectionId & DIR_LEFT) {
        validAngles.push_back(B2_PI);
    }
    if (intersectionId & DIR_UP) {
        validAngles.push_back(-B2_PI / 2.0f);
    }
    if (intersectionId & DIR_DOWN) {
        validAngles.push_back(B2_PI / 2.0f);
    }

    if (validAngles.empty())
        return;

    int choice = std::rand() % validAngles.size();
    float newAngle = validAngles[choice];

    b2Vec2 currentPos = b2Body_GetPosition(body);
    b2Body_SetTransform(body, currentPos, b2MakeRot(newAngle));
}

bool NPCCar::isNPCBlocked() { return isBlocked; }

uint8_t NPCCar::getCarType() const { return carType; }

void NPCCar::applyCollision(const CollisionInfo& info) {
    // Call parent collision handler for damage
    Car::applyCollision(info);
    
    // Activate retroceso mode
    if (!isInRetrocesoMode) {
        isInRetrocesoMode = true;
        retrocesoFramesRemaining = RETROCESO_FRAMES;
        retrocesoAngle = b2Rot_GetAngle(b2Body_GetRotation(body)) + (B2_PI / 2.0f) * ((std::rand() % 2) * 2 - 1);
    }
}
