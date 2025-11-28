#ifndef WORLD_CONTACT_LISTENER_H
#define WORLD_CONTACT_LISTENER_H

#include <box2d/box2d.h>

#include "Car.h"
#include "CheckpointManager.h"

class WorldContactListener {
private:
    CheckpointManager& checkpointManager;

    void processCarCollision(Car& carA, Car& carB, float approachSpeed, float deltaTime);

    float getImpactForce(Car& carA, Car& carB, float approachSpeed, float deltaTime);

    float getImpactAngle(Car& carA, Car& carB);

public:
    explicit WorldContactListener(CheckpointManager& checkpointManager);

    void BeginContact(b2ContactHitEvent* contact, float deltaTime);

    void BeginTouch(const b2SensorBeginTouchEvent* sensor);

    void EndTouch(const b2SensorEndTouchEvent* sensor);

    ~WorldContactListener();
};


#endif
