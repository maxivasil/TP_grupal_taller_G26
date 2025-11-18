#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine(CheckpointManager& checkpointManager): listener(checkpointManager) {
    b2WorldDef def = b2DefaultWorldDef();
    def.gravity = b2Vec2_zero;

    world = b2CreateWorld(&def);

    b2World_EnableContinuous(world, true);
}

void PhysicsEngine::handle_collisions(b2ContactHitEvent* collisions, int count, float dt) {
    for (int i = 0; i < count; i++, collisions++) {
        listener.BeginContact(collisions, dt);
    }
}

void PhysicsEngine::handle_sensors(b2SensorBeginTouchEvent* sensors, int count) {
    for (int i = 0; i < count; i++, sensors++) {
        listener.BeginTouch(sensors);
    }
}

void PhysicsEngine::step(float dt, int velocityIterations) {
    b2World_Step(world, dt, velocityIterations);
    b2ContactEvents collisions = b2World_GetContactEvents(world);
    b2SensorEvents sensors = b2World_GetSensorEvents(world);
    handle_collisions(collisions.hitEvents, collisions.hitCount, dt);
    handle_sensors(sensors.beginEvents, sensors.beginCount);
}

b2WorldId PhysicsEngine::getWorld() const { return world; }

PhysicsEngine::~PhysicsEngine() { b2DestroyWorld(world); }
