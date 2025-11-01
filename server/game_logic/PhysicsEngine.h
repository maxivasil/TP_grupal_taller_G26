#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H

#include <box2d/box2d.h>
#include "world_contact_listener.h"

class PhysicsEngine {
private:
    b2WorldId world;
    WorldContactListener listener;

    void handle_collisions(b2ContactHitEvent* collisions, int beginCount, float dt);

    void handle_sensors(b2SensorBeginTouchEvent* collisions, int beginCount);

public:
    explicit PhysicsEngine(CheckpointManager& checkpointManager);

    void step(float dt, int velocityIterations);

    b2WorldId getWorld() const;

    ~PhysicsEngine();
};


#endif
