#include "Car.h"

#include <algorithm>

#include "CollisionCategories.h"

#define DAMAGE_SCALING_FACTOR 100000

#define FRONTAL_COLLISION_FACTOR 1.0f
#define LATERAL_COLLISION_FACTOR 0.7f
#define REAR_COLLISION_FACTOR 0.3f

#define MIN_IMPACT_FORCE 0.001f

b2BodyDef Car::initCarBodyDef(b2Vec2 position, b2Rot rotation) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    bodyDef.rotation = rotation;
    bodyDef.userData = this;
    bodyDef.isAwake = true;
    bodyDef.linearDamping = 0.7f;
    bodyDef.angularDamping = 1.0f;
    return bodyDef;
}

void Car::setShape(b2BodyId body) {
    b2Polygon polygon = b2MakeBox(stats.width / 2, stats.length / 2);
    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.enableHitEvents = true;
    shape_def.enableSensorEvents = true;
    shape_def.density = stats.mass / (stats.width * stats.length);
    shape_def.isSensor = false;
    shape_def.filter.categoryBits = CollisionCategories::CategoryGroundCar;
    shape_def.filter.maskBits =
            CollisionCategories::CategoryGroundObj | CollisionCategories::CategoryGroundCar |
            CollisionCategories::CategoryCheckpoint | CollisionCategories::CategoryBridgeSensor;
    b2ShapeId shape = b2CreatePolygonShape(body, &shape_def, &polygon);
    b2Shape_EnableContactEvents(shape, true);
    b2Body_ApplyMassFromShapes(body);
}

Car::Car(b2WorldId world, const CarStats& stats_, b2Vec2 position, b2Rot rotation):
        stats(stats_), hasInfiniteHealth(false), isOnBridge(false), reverseMode(false) {
    b2BodyDef bodyDef = initCarBodyDef(position, rotation);
    body = b2CreateBody(world, &bodyDef);

    setShape(body);

    current_health = stats_.health_max;
}

void Car::handleAccelerating(bool accelerating, float speed, b2Vec2 forward) {
    if (accelerating && speed < getMaxSpeed()) {
        b2Vec2 force = b2MulSV(b2Body_GetMass(body) * getAcceleration(), forward);
        b2Body_ApplyForceToCenter(body, force, true);
    }
}

void Car::handleBraking(bool braking, b2Vec2 velocity) {
    float speed = b2Length(velocity);
    const float reverseThreshold = 15.0f;

    if (braking) {
        if (reverseMode) {
            float reverseSpeedFactor = std::clamp(speed / getMaxSpeed(), 0.5f, 1.0f);
            b2Vec2 backward = b2RotateVector(b2Body_GetRotation(body), {-1, 0});
            b2Vec2 reverse_force = b2MulSV(
                    b2Body_GetMass(body) * (getAcceleration() * reverseSpeedFactor), backward);
            b2Body_ApplyForceToCenter(body, reverse_force, true);
        } else {
            if (speed > reverseThreshold) {
                b2Vec2 brake_force =
                        b2MulSV(b2Body_GetMass(body) * stats.brake_force, -b2Normalize(velocity));
                b2Body_ApplyForceToCenter(body, brake_force, true);
            } else {
                reverseMode = true;
            }
        }
    }

    if (reverseMode && !braking &&
        b2Dot(velocity, b2RotateVector(b2Body_GetRotation(body), {1, 0})) > 0) {
        reverseMode = false;
    }
}

void Car::handleTurning(Direction turn_direction, float speed) {
    const float minSpeedForTurning = 0.05f;

    if (speed < minSpeedForTurning || turn_direction == Direction::FORWARD) {
        return;
    }

    b2Vec2 velocity = b2Body_GetLinearVelocity(body);
    b2Vec2 forward = b2RotateVector(b2Body_GetRotation(body), {1, 0});
    float dot = b2Dot(velocity, forward);

    bool movingBackward = (dot < -0.2f);

    int turn = (turn_direction == Direction::LEFT) ? -1 : 1;

    if (movingBackward) {
        turn = -turn;
    }

    float speedFactor = std::clamp(speed / getMaxSpeed(), 0.3f, 1.0f);
    float torque = turn * stats.turn_speed * getHandling() * speedFactor * 1700.0f;
    b2Body_ApplyTorque(body, torque, true);
}

void Car::verifyMaxSpeed(b2Vec2 velocity, float speed) {
    if (speed > getMaxSpeed()) {
        b2Vec2 limited = b2Normalize(velocity) * getMaxSpeed();
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

    int infiniteHealthFactor = hasInfiniteHealth ? 0 : 1;
    float damage =
            infiniteHealthFactor * info.impactForce * angle_damage_factor / DAMAGE_SCALING_FACTOR;
    current_health -= damage;
    if (current_health < 0) {
        current_health = 0;
    }

    b2Vec2 vel = b2Body_GetLinearVelocity(body);
    float speed = b2Length(vel);
    if (speed > 1e-6f) {
        float velocityLoss = std::min(speed, damage * 0.1f);
        b2Body_SetLinearVelocity(body, vel * ((speed - velocityLoss) / speed));
    } else {
        b2Body_SetLinearVelocity(body, {0.0f, 0.0f});
    }
}

void Car::repair() { current_health = getMaxHealth(); }

bool Car::isDestroyed() const { return current_health <= 0; }
B2_API b2BodyId b2CreateBody(b2WorldId worldId, const b2BodyDef* def);

b2Vec2 Car::getPosition() const { return b2Body_GetPosition(body); }

b2Vec2 Car::getLinearVelocity() const { return b2Body_GetLinearVelocity(body); }

float Car::getSpeed() const {
    b2Vec2 velocity = b2Body_GetLinearVelocity(body);
    return std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
}

b2Rot Car::getRotation() const { return b2Body_GetRotation(body); }

float Car::getCurrentHealth() const { return current_health; }

float Car::getMass() const { return b2Body_GetMass(body); }

float Car::getImpactForce(const Collidable* other, float approachSpeed, float deltaTime) {
    float mA = getMass();
    float mB = other->getMass();
    float reducedMass = (mA * mB) / (mA + mB);

    return reducedMass * approachSpeed / deltaTime;
}

float Car::getImpactAngle(const Collidable* other, const b2Vec2& contactNormal) {
    b2Vec2 forwardA = b2RotateVector(getRotation(), {1, 0});
    b2Vec2 forwardB;

    if (auto otherCar = dynamic_cast<const Car*>(other)) {
        forwardB = b2RotateVector(otherCar->getRotation(), {1, 0});
    } else {
        forwardB = -b2Normalize(contactNormal);
    }

    float dot = b2Dot(b2Normalize(forwardA), b2Normalize(forwardB));
    dot = std::clamp(dot, -1.0f, 1.0f);
    return std::acos(dot);
}

void Car::onCollision(Collidable* other, float approachSpeed, float deltaTime,
                      const b2Vec2& contactNormal) {
    float impactForce = getImpactForce(other, approachSpeed, deltaTime);

    if (impactForce < MIN_IMPACT_FORCE) {
        return;
    }

    CollisionInfo info;
    info.impactForce = impactForce;
    info.angle = getImpactAngle(other, contactNormal);

    applyCollision(info);
}

b2Rot Car::getRotation([[maybe_unused]] const b2Vec2& contactNormal) const { return getRotation(); }

void Car::setInfiniteHealth() { hasInfiniteHealth = true; }

bool Car::hasInfiniteHealthActive() const { return hasInfiniteHealth; }

void Car::setLevel(bool onBridge) {
    isOnBridge = onBridge;

    b2ShapeId shapes[1];
    int count = b2Body_GetShapes(body, shapes, 1);
    if (count > 0) {
        b2Filter filter = b2Shape_GetFilter(shapes[0]);
        if (onBridge) {
            filter.categoryBits = CollisionCategories::CategoryBridgeCar;
            filter.maskBits = CollisionCategories::CategoryBridgeObj |
                              CollisionCategories::CategoryBridgeCar |
                              CollisionCategories::CategoryCheckpoint |
                              CollisionCategories::CategoryBridgeSensor;
        } else {
            filter.categoryBits = CollisionCategories::CategoryGroundCar;
            filter.maskBits = CollisionCategories::CategoryGroundObj |
                              CollisionCategories::CategoryGroundCar |
                              CollisionCategories::CategoryCheckpoint |
                              CollisionCategories::CategoryBridgeSensor;
        }
        b2Shape_SetFilter(shapes[0], filter);
    }
}

bool Car::getIsOnBridge() const { return isOnBridge; }

void Car::applyUpgrades(const CarUpgrades& newUpgrades) {
    if (!newUpgrades.isValid()) {
        return;  // Rechazar upgrades inválidos
    }

    upgrades = newUpgrades;

    // Actualizar la salud máxima si cambia
    float newMaxHealth = stats.health_max + upgrades.health_boost;
    if (newMaxHealth > 0) {
        current_health = current_health == stats.health_max ? newMaxHealth : current_health;
    }
}

const CarUpgrades& Car::getUpgrades() const { return upgrades; }

float Car::getMaxSpeed() const { return stats.max_speed + upgrades.speed_boost; }

float Car::getAcceleration() const { return stats.acceleration + upgrades.acceleration_boost; }

float Car::getHandling() const { return stats.handling + upgrades.handling_boost; }

float Car::getMaxHealth() const { return stats.health_max + upgrades.health_boost; }

void Car::setDestroyed() { current_health = 0.0f; }

Car::~Car() {}
