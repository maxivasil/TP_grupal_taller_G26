#ifndef NPC_CAR_H
#define NPC_CAR_H

#include "Car.h"
#include <box2d/box2d.h>


class NPCCar : public Car {  
private: 
    bool isParked;
    bool isBlocked;
    b2Vec2 lastPos = {0, 0};
    int blockedFrames = 0;

    void handleBlocked();

public:
    NPCCar(b2WorldId world, const CarStats& stats, b2Vec2 position, b2Rot rotation, bool parked);
    ~NPCCar() override = default;

    void updatePhysics(const CarInput& input) override;

    bool isNPCBlocked();
};

#endif
