#include "StaticObject.h"

b2BodyDef StaticObject::initStaticObjectBodyDef(b2Vec2 position) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    bodyDef.userData = this;
    bodyDef.isAwake = true;
    bodyDef.linearVelocity = b2Vec2_zero;
    return bodyDef;
}

void StaticObject::setShape(b2BodyId body, float width, float length, float mass) {
    b2Polygon polygon = b2MakeBox(width / 2, length / 2);
    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.enableHitEvents = true;
    shape_def.density = mass / (width * length);
    b2ShapeId shape = b2CreatePolygonShape(body, &shape_def, &polygon);
    b2Shape_EnableContactEvents(shape, true);
}

StaticObject::StaticObject(b2WorldId world, b2Vec2 position, StaticObjectParam& params) {
    b2BodyDef bodyDef = initStaticObjectBodyDef(position);
    body = b2CreateBody(world, &bodyDef);

    setShape(body, params.width, params.length, params.mass);
}

void StaticObject::onCollision([[maybe_unused]] Collidable* other, [[maybe_unused]] float approachSpeed, [[maybe_unused]] float deltaTime, [[maybe_unused]] const b2Vec2& contactNormal) {}

float StaticObject::getMass() const {
    return b2Body_GetMass(body);
}

b2Rot StaticObject::getRotation(const b2Vec2& contactNormal) const {
    b2Vec2 normalized = b2Normalize(contactNormal);
    float angle = b2Atan2(normalized.y, normalized.x);
    return b2MakeRot(angle);
}

void StaticObject::applyCollision([[maybe_unused]] const CollisionInfo& info) {}

StaticObject::~StaticObject() {}

