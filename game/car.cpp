#include "car.h"
#include "world.h"
#include <algorithm>
#include <box2d/box2d.h>
#include <SDL2/SDL.h>
#include <iostream>
#include "collision_categories.h"

#define DAMAGE_SCALING_FACTOR 100000

#define FRONTAL_COLLISION_FACTOR 1.0f
#define LATERAL_COLLISION_FACTOR 0.7f
#define REAR_COLLISION_FACTOR 0.3f

#define MIN_IMPACT_FORCE 0.001f

b2BodyDef Car::initCarBodyDef(b2Vec2 position, b2Rot rotation) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    bodyDef.rotation = rotation;  // Asegurate que esto esté
    bodyDef.linearDamping = 1.0f;
    bodyDef.angularDamping = 2.0f;
    return bodyDef;
}
Car::Car(b2WorldId world, const CarStats& stats_, b2Vec2 position, b2Rot rotation):
        stats(stats_), hasInfiniteHealth(false), isOnBridge(false) {
    
    b2BodyDef bodyDef = initCarBodyDef(position, rotation);
    body = b2CreateBody(world, &bodyDef);
    setShape(body);
    current_health = stats.health_max;
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
    
    
    b2Body_SetLinearDamping(body, 0.3f);  
    b2Body_SetAngularDamping(body, 0.5f);  
}

void Car::handleAccelerating(bool accelerating, float speed, b2Vec2 forward) {
    if (accelerating && speed < stats.max_speed) {
        b2Vec2 force = b2MulSV(stats.acceleration * b2Body_GetMass(body), forward);
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
    if (speed > 0.1f && turn_direction != Direction::NONE) {
        int turn = 0;
        switch (turn_direction) {
            case Direction::LEFT:
                turn = -1;
                break;
            case Direction::RIGHT:
                turn = 1;
                break;
            case Direction::NONE:
            default:
                turn = 0;
                break;
        }
        // Reducir el factor multiplicador
        float torque = turn * stats.turn_speed * stats.handling;  
        b2Body_SetAngularVelocity(body, torque);
    } else {
        b2Body_SetAngularVelocity(body, 0.0f);
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
    
    // Forward = hacia donde apunta el auto (norte en coordenadas locales)
    b2Rot rot = b2Body_GetRotation(body);
    b2Vec2 forward = b2RotateVector(rot, {0.0f, -1.0f});  // Hacia arriba (norte)

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

void Car::update(float deltaTime) {
    const Uint8* state = SDL_GetKeyboardState(NULL);
    const float accel = 300.0f * deltaTime;
    const float decel = 200.0f * deltaTime;
    const float turnSpeed = 120.0f * deltaTime;
    
    float speed = b2Length(b2Body_GetLinearVelocity(body));

    if (state[SDL_SCANCODE_W]) speed += accel;
    else if (state[SDL_SCANCODE_S]) speed -= accel;
    float angle = b2Rot_GetAngle(b2Body_GetRotation(body)) * 180.0f / B2_PI;
    if (state[SDL_SCANCODE_A]) angle -= turnSpeed;
    if (state[SDL_SCANCODE_D]) angle += turnSpeed;

    b2Vec2 pos = b2Body_GetPosition(body);
    float x = pos.x;
    float y = pos.y;
    x += cosf(angle * M_PI / 180.0f) * speed * deltaTime;
    y += sinf(angle * M_PI / 180.0f) * speed * deltaTime;
    x += cosf(angle * M_PI / 180.0f) * speed * deltaTime;
    y += sinf(angle * M_PI / 180.0f) * speed * deltaTime;
}

void Car::render(SDL_Renderer* renderer, const SDL_FRect& camera) const {
    b2Vec2 pos = b2Body_GetPosition(body);
    b2Rot rot = b2Body_GetRotation(body);
    
    // Definir triángulo apuntando HACIA ARRIBA (norte = Y negativo)
    float halfWidth = 10.0f;
    float halfLength = 15.0f;
    
    b2Vec2 localPoints[3] = {
        {0.0f, -halfLength},        // Punta delantera (norte)
        {-halfWidth, halfLength},   // Trasera izquierda
        {halfWidth, halfLength}     // Trasera derecha
    };
    
    // Rotar y trasladar cada punto
    SDL_Vertex vertices[3];
    for (int i = 0; i < 3; ++i) {
        // Rotar el punto local con la rotación del body
        b2Vec2 rotated = b2RotateVector(rot, localPoints[i]);
        
        // Trasladar a posición mundial y luego a pantalla
        vertices[i].position.x = pos.x + rotated.x - camera.x;
        vertices[i].position.y = pos.y + rotated.y - camera.y;
        vertices[i].color = {255, 60, 60, 255};  // Rojo
        vertices[i].tex_coord = {0, 0};
    }
    
    int indices[3] = {0, 1, 2};
    SDL_RenderGeometry(renderer, nullptr, vertices, 3, indices, 3);
    
    // Punto amarillo en el frente (ya rotado)
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_FRect frontDot = {
        vertices[0].position.x - 3,
        vertices[0].position.y - 3,
        6, 6
    };
    SDL_RenderFillRectF(renderer, &frontDot);
}

float Car::getAngle() const {
    b2Rot rotation = b2Body_GetRotation(body);
    return b2Rot_GetAngle(rotation);
}
void Car::setInfiniteHealth() { hasInfiniteHealth = true; }

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

Car::~Car() {}
