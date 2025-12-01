#include "NPCCar.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>

#define MINMOVEMENT 0.03f
#define MAXFRAMESBLOQUED 150
#define RETROCESO_FRAMES 90  // ~1.5 segundos a 60 FPS
#define INTERSECTION_COOLDOWN 90
#define POST_TURN_GRACE 60

NPCCar::NPCCar(b2WorldId world, const CarStats& stats, b2Vec2 position, b2Rot rotation, bool parked,
               uint8_t carType):
        Car(world, stats, position, rotation),
        isParked(parked),
        isBlocked(false),
        carType(carType) {}

CarInput NPCCar::generateRandomMovement() {
    float speed = b2Length(b2Body_GetLinearVelocity(body));
    float normalized = std::clamp(speed / getMaxSpeed(), 0.0f, 1.0f);

    float accelProb = (1.0f - normalized) * 0.2f;
    float brakeProb = normalized * 0.7f;

    bool accelerating = ((std::rand() / float(RAND_MAX)) < accelProb);
    if (accelerating) {
        brakeProb *= 0.3f;
    }
    bool braking = ((std::rand() / float(RAND_MAX)) < brakeProb);

    return {.accelerating = accelerating, .braking = braking, .turn_direction = Direction::FORWARD};
}

void NPCCar::updatePhysics(const CarInput& input) {
    if (intersectionCooldownFrames > 0)
        intersectionCooldownFrames--;
    if (postTurnGraceFrames > 0)
        postTurnGraceFrames--;
    if (isParked)
        return;
    if (isBlocked) {
        b2Body_SetLinearVelocity(body, {0, 0});
        b2Body_SetAngularVelocity(body, 0);
        b2Body_SetAwake(body, false);
        return;
    }
    if (isInRetrocesoMode) {
        retrocesoFramesRemaining--;

        if (retrocesoFramesRemaining > RETROCESO_FRAMES / 2) {
            // Primera mitad: retroceder
            Car::updatePhysics(
                    {.accelerating = false, .braking = true, .turn_direction = Direction::FORWARD});
        } else if (retrocesoFramesRemaining > 0) {
            // Segunda mitad: frenar completamente
            Car::updatePhysics(
                    {.accelerating = false, .braking = true, .turn_direction = Direction::FORWARD});
            // Detener completamente
            b2Body_SetLinearVelocity(body, {0.0f, 0.0f});
        } else {
            // Terminó retroceso: elegir una dirección válida de la intersección
            isInRetrocesoMode = false;

            // Si tenemos info de intersección válida, usar solo direcciones válidas
            if (lastIntersectionId > 0) {
                chooseIntersectionDirection(lastIntersectionId);
            } else {
                // Fallback: si no hay intersección, girar aleatoriamente
                float currentHeading = b2Rot_GetAngle(b2Body_GetRotation(body));
                float newHeading =
                        currentHeading + (std::rand() % 2 == 0 ? B2_PI / 2.0f : -B2_PI / 2.0f);
                b2Vec2 pos = b2Body_GetPosition(body);
                b2Body_SetTransform(body, pos, b2MakeRot(newHeading));
            }
        }
        handleBlocked();
        return;
    }

    CarInput npcMovement = generateRandomMovement();
    Car::updatePhysics(npcMovement);

    if (!isParked) {
        handleBlocked();
    }
}

void NPCCar::handleBlocked() {
    if (intersectionCooldownFrames > 0 || postTurnGraceFrames > 0) {
        blockedFrames = 0;
        isBlocked = false;
        lastPos = b2Body_GetPosition(body);
        return;
    }

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
    if (intersectionCooldownFrames > 0)
        return;
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

    float currentAngle = b2Rot_GetAngle(b2Body_GetRotation(body));

    int choice = std::rand() % validAngles.size();
    float newAngle = validAngles[choice];

    if (fabsf(fmodf(newAngle - currentAngle + B2_PI, 2 * B2_PI) - B2_PI) < 0.01f) {
        int secondChoice = std::rand() % validAngles.size();
        newAngle = validAngles[secondChoice];
    }

    b2Vec2 currentPos = b2Body_GetPosition(body);
    b2Body_SetLinearVelocity(body, {0.0f, 0.0f});
    b2Body_SetTransform(body, currentPos, b2MakeRot(newAngle));

    intersectionCooldownFrames = INTERSECTION_COOLDOWN;
    postTurnGraceFrames = POST_TURN_GRACE;
}

bool NPCCar::isNPCBlocked() { return isBlocked; }

uint8_t NPCCar::getCarType() const { return carType; }

void NPCCar::respawn(b2Vec2 pos, b2Rot rot) {
    b2Body_SetTransform(body, pos, rot);
    b2Body_SetLinearVelocity(body, {0.0f, 0.0f});
    b2Body_SetAngularVelocity(body, 0.0f);
    b2Body_SetAwake(body, true);
    setLevel(false);
    blockedFrames = 0;
    isBlocked = false;
    isInRetrocesoMode = false;
    intersectionCooldownFrames = 0;
    postTurnGraceFrames = 0;
    repair();
}

void NPCCar::applyCollision(const CollisionInfo& info) {
    // Call parent collision handler for damage
    Car::applyCollision(info);

    // Activate retroceso mode
    if (!isInRetrocesoMode) {
        isInRetrocesoMode = true;
        retrocesoFramesRemaining = RETROCESO_FRAMES;
        retrocesoAngle = b2Rot_GetAngle(b2Body_GetRotation(body)) +
                         (B2_PI / 2.0f) * ((std::rand() % 2) * 2 - 1);
    }
}
