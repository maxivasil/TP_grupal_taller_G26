#ifndef NPC_CAR_H
#define NPC_CAR_H

#include <box2d/box2d.h>

#include "Car.h"
#include "SensorData.h"


class NPCCar: public Car {
private:
    bool isParked;
    bool isBlocked;
    b2Vec2 lastPos = {0, 0};
    int blockedFrames = 0;
    uint8_t carType;
    bool isInRetrocesoMode = false;
    int retrocesoFramesRemaining = 0;
    float retrocesoAngle = 0.0f;

    void handleBlocked();

public:
    NPCCar(b2WorldId world, const CarStats& stats, b2Vec2 position, b2Rot rotation, bool parked,
           uint8_t carType);
    ~NPCCar() override = default;

    void updatePhysics(const CarInput& input) override;

    void chooseIntersectionDirection(int intersectionId) override;

    void applyCollision(const CollisionInfo& info) override;

    bool isNPCBlocked();

    uint8_t getCarType() const;
};

#endif
