#include "WorldContactListener.h"

#include <algorithm>

#include "Collidable.h"
#include "SensorData.h"

WorldContactListener::WorldContactListener(CheckpointManager& checkpointManager):
        checkpointManager(checkpointManager) {}

void WorldContactListener::BeginContact(b2ContactHitEvent* contact, float deltaTime) {
    b2BodyId bodyA = b2Shape_GetBody(contact->shapeIdA);
    b2BodyId bodyB = b2Shape_GetBody(contact->shapeIdB);

    Collidable* objA = static_cast<Collidable*>(b2Body_GetUserData(bodyA));
    Collidable* objB = static_cast<Collidable*>(b2Body_GetUserData(bodyB));

    if (!objA || !objB)
        return;

    objA->onCollision(objB, contact->approachSpeed, deltaTime, contact->normal);
    objB->onCollision(objA, contact->approachSpeed, deltaTime, -contact->normal);
}

void WorldContactListener::BeginTouch(const b2SensorBeginTouchEvent* sensor) {
    b2ShapeId sensorShape = sensor->sensorShapeId;
    b2ShapeId otherShape = sensor->visitorShapeId;

    void* userData = b2Shape_GetUserData(sensorShape);
    if (!userData) {
        return;
    }

    const SensorData* data = static_cast<SensorData*>(userData);

    b2BodyId otherBody = b2Shape_GetBody(otherShape);
    Car* car = reinterpret_cast<Car*>(b2Body_GetUserData(otherBody));
    if (!car)
        return;

    switch (data->type) {
        case SensorType::Checkpoint:
            checkpointManager.onCarEnterCheckpoint(car, data->id);
            break;
        case SensorType::BridgeLevel:
            car->setLevel(true);  // Entró al sensor de puente
            break;
    }
}

void WorldContactListener::EndTouch(const b2SensorEndTouchEvent* sensor) {
    b2ShapeId sensorShape = sensor->sensorShapeId;
    b2ShapeId otherShape = sensor->visitorShapeId;

    void* userData = b2Shape_GetUserData(sensorShape);
    if (!userData) {
        return;
    }

    const SensorData* data = static_cast<SensorData*>(userData);

    b2BodyId otherBody = b2Shape_GetBody(otherShape);
    Car* car = reinterpret_cast<Car*>(b2Body_GetUserData(otherBody));
    if (!car)
        return;

    switch (data->type) {
        case SensorType::Checkpoint:
            // No hacer nada en Checkpoint cuando sale
            break;
        case SensorType::BridgeLevel:
            car->setLevel(false);  // Salió del sensor de puente
            break;
    }
}

WorldContactListener::~WorldContactListener() {}
