#include "WorldContactListener.h"
#include <algorithm>

#define MIN_IMPACT_FORCE 0.001f

WorldContactListener::WorldContactListener() {}

void WorldContactListener::BeginContact(b2ContactHitEvent* contact, float deltaTime) {
    b2BodyId bodyA = b2Shape_GetBody(contact->shapeIdA);
    b2BodyId bodyB = b2Shape_GetBody(contact->shapeIdB);

    void* userDataA = b2Body_GetUserData(bodyA);
    void* userDataB = b2Body_GetUserData(bodyB);

    Car* carA = static_cast<Car*>(userDataA);
    Car* carB = static_cast<Car*>(userDataB);

    if (carA && carB) {
        processCarCollision(*carA, *carB, contact->approachSpeed, deltaTime);
    }
    // else if (carA || carB) {
        // Choque contra pared u objeto estático
        //Car* car = carA ? carA : carB;
        //processWallCollision(*car, contact);
    //}
}

float WorldContactListener::getImpactForce(Car& carA, Car& carB, float approachSpeed, float deltaTime){
    float mA = carA.getMass();
    float mB = carB.getMass();
    float reducedMass = (mA * mB) / (mA + mB);

    return reducedMass * approachSpeed / deltaTime;
}

float WorldContactListener::getImpactAngle(Car& carA, Car& carB){
    b2Vec2 forwardA = b2RotateVector(carA.getRotation(), {1, 0});
    b2Vec2 forwardB = b2RotateVector(carB.getRotation(), {1, 0});

    float dot = b2Dot(b2Normalize(forwardA), b2Normalize(forwardB));
    dot = std::clamp(dot, -1.0f, 1.0f);
    return std::acos(dot);
}

void WorldContactListener::processCarCollision(Car& carA, Car& carB, float approachSpeed, float deltaTime) {
    float impactForce = getImpactForce(carA, carB, approachSpeed, deltaTime);
    
    if (impactForce < MIN_IMPACT_FORCE){
        return;
    }

    CollisionInfo info;
    info.impactForce = impactForce;
    info.angle = getImpactAngle(carA, carB);

    carA.applyCollision(info);
    carB.applyCollision(info);
}

WorldContactListener::~WorldContactListener() {}
