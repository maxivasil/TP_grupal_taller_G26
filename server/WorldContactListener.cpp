#include "WorldContactListener.h"
#include "Collidable.h"
#include <algorithm>


WorldContactListener::WorldContactListener() {}

void WorldContactListener::BeginContact(b2ContactHitEvent* contact, float deltaTime) {
    b2BodyId bodyA = b2Shape_GetBody(contact->shapeIdA);
    b2BodyId bodyB = b2Shape_GetBody(contact->shapeIdB);

    Collidable* objA = static_cast<Collidable*>(b2Body_GetUserData(bodyA));
    Collidable* objB = static_cast<Collidable*>(b2Body_GetUserData(bodyB));

    if (!objA || !objB) return;

    objA->onCollision(objB, contact->approachSpeed, deltaTime, contact->normal);
    objB->onCollision(objA, contact->approachSpeed, deltaTime, -contact->normal);
}

WorldContactListener::~WorldContactListener() {}
