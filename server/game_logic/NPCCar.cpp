#include "NPCCar.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>

#define MINMOVEMENT 0.1f
#define MAXFRAMESBLOQUED 60

NPCCar::NPCCar(b2WorldId world, const CarStats& stats, b2Vec2 position, b2Rot rotation, bool parked,
               uint8_t carType):
        Car(world, stats, position, rotation),
        isParked(parked),
        isBlocked(false),
        carType(carType) {}

void NPCCar::updatePhysics(const CarInput& input) {
    b2Vec2 forward = b2Normalize(b2RotateVector(b2Body_GetRotation(body), {1, 0}));
    forward = b2MulSV(getMaxSpeed(), forward);
    b2Body_SetLinearVelocity(body, forward);

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
