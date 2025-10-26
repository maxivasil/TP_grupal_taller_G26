#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine() {
    b2WorldDef def = b2DefaultWorldDef();
    def.gravity = b2Vec2_zero;
    world = b2CreateWorld(&def);

    b2World_EnableContinuous(world, true);
}

void PhysicsEngine::handle_collisions(b2ContactHitEvent* collisions, int beginCount, float dt){
    for (int i = 0; i < beginCount; i++, collisions++) {
        listener.BeginContact(collisions, dt);
    }
}

void PhysicsEngine::step(float dt, int velocityIterations){
    b2World_Step(world, dt, velocityIterations);
    b2ContactEvents collisions = b2World_GetContactEvents(world);
    handle_collisions(collisions.hitEvents, collisions.hitCount, dt);
}

b2WorldId PhysicsEngine::getWorld() const {
    return world;
}

PhysicsEngine::~PhysicsEngine() {
    b2DestroyWorld(world);
}
