#include "NPCCar.h"
#include <algorithm>
#include <cmath>

#define MINMOVEMENT 0.1f
#define MAXFRAMESBLOQUED 60

NPCCar::NPCCar(b2WorldId world, const CarStats& stats, b2Vec2 position, b2Rot rotation, bool parked) : Car(world, stats, position, rotation), isParked(parked) {}

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

bool NPCCar::isNPCBlocked() {
    return isBlocked;
}

NPCCar::~NPCCar() {}
