#include "Car.h"

#include <algorithm>

#define DAMAGE_SCALING_FACTOR 500

#define FRONTAL_COLLISION_FACTOR 1.0f
#define LATERAL_COLLISION_FACTOR 0.7f
#define REAR_COLLISION_FACTOR 0.3f

b2BodyDef Car::initCarBodyDef(b2Vec2 position, b2Rot rotation) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    bodyDef.rotation = rotation;
    bodyDef.userData = this;
    bodyDef.isAwake = true;
    return bodyDef;
}

void Car::setShape(b2BodyId body, float width, float length) {
    b2Polygon polygon = b2MakeBox(width / 2, length / 2);
    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.enableHitEvents = true;
    b2ShapeId shape = b2CreatePolygonShape(body, &shape_def, &polygon);
    b2Shape_EnableContactEvents(shape, true);
    b2Body_ApplyMassFromShapes(body);
}

Car::Car(b2WorldId world, const CarStats&& stats_, b2Vec2 position, b2Rot rotation) {
    b2BodyDef bodyDef = initCarBodyDef(position, rotation);
    body = b2CreateBody(world, &bodyDef);

    setShape(body, stats_.width, stats_.length);

    stats = stats_;
    current_health = stats_.health_max;
}

void Car::handleAccelerating(bool accelerating, float speed, b2Vec2 forward) {
    if (accelerating && speed < stats.max_speed) {
        b2Vec2 force = b2MulSV(b2Body_GetMass(body) * stats.acceleration, forward);
        b2Body_ApplyForceToCenter(body, force, true);
    }
}

void Car::handleBraking(bool braking, b2Vec2 velocity) {
    if (braking) {
        b2Vec2 brake_force =
                b2MulSV(b2Body_GetMass(body) * stats.brake_force, -b2Normalize(velocity));
        b2Body_ApplyForceToCenter(body, brake_force, true);
    }
}

void Car::handleTurning(Direction turn_direction, float speed) {
    if (speed > 0.1f && turn_direction != Direction::FORWARD) {
        int turn = 0;
        switch (turn_direction) {
            case Direction::LEFT:
                turn = -1;
                break;
            case Direction::RIGHT:
                turn = 1;
                break;
            case Direction::FORWARD:
            default:
                turn = 0;
                break;
        }
        float torque = turn * stats.turn_speed * (stats.handling);
        b2Body_SetAngularVelocity(body, torque);
    }
}

void Car::verifyMaxSpeed(b2Vec2 velocity, float speed) {
    if (speed > stats.max_speed) {
        b2Vec2 limited = b2Normalize(velocity) * stats.max_speed;
        b2Body_SetLinearVelocity(body, limited);
    }
}

void Car::updatePhysics(const CarInput& input) {
    b2Vec2 velocity = b2Body_GetLinearVelocity(body);
    float speed = b2Length(velocity);
    b2Vec2 forward = b2Normalize(b2RotateVector(b2Body_GetRotation(body), {1, 0}));

    handleAccelerating(input.accelerating, speed, forward);
    handleBraking(input.braking, velocity);
    handleTurning(input.turn_direction, speed);
    verifyMaxSpeed(velocity, speed);
}

float getCollisionAngleDamageFactor(float angle_rad) {
    float angle_deg = angle_rad * (180.0f / B2_PI);

    float angle_damage_factor = REAR_COLLISION_FACTOR;
    if (angle_deg > 30.0f && angle_deg < 120.0f) {
        angle_damage_factor = LATERAL_COLLISION_FACTOR;
    } else if (angle_deg > 120.0f) {
        angle_damage_factor = FRONTAL_COLLISION_FACTOR;
    }
    return angle_damage_factor;
}

void Car::applyCollision(const CollisionInfo& info) {
    float angle_damage_factor = getCollisionAngleDamageFactor(info.angle);

    float damage = info.impactForce * angle_damage_factor / DAMAGE_SCALING_FACTOR;
    current_health -= damage;
    if (current_health < 0) {
        current_health = 0;
    }

    b2Vec2 vel = b2Body_GetLinearVelocity(body);
    float speed = b2Length(vel);
    float velocityLoss = std::min(speed, damage * 0.1f);
    b2Body_SetLinearVelocity(body, vel * ((speed - velocityLoss) / speed));

    // ver de hacer que segun el damage se notifique para que se
    // haga la animacion de choque
}

void Car::repair() { current_health = stats.health_max; }

bool Car::isDestroyed() const { return current_health <= 0; }
B2_API b2BodyId b2CreateBody(b2WorldId worldId, const b2BodyDef* def);

b2Vec2 Car::getPosition() const { return b2Body_GetPosition(body); }

b2Vec2 Car::getLinearVelocity() const { return b2Body_GetLinearVelocity(body); }

b2Rot Car::getRotation() const { return b2Body_GetRotation(body); }

float Car::getCurrentHealth() const { return current_health; }

float Car::getMass() const { return b2Body_GetMass(body); }

Car::~Car() {}
