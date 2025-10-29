#include "static_object.h"

#include "CollisionCategories.h"

b2BodyDef StaticObject::initStaticObjectBodyDef(b2Vec2 position) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_staticBody;
    bodyDef.position = position;
    bodyDef.userData = this;
    bodyDef.isAwake = true;
    bodyDef.linearVelocity = b2Vec2_zero;
    return bodyDef;
}

void StaticObject::setShape(b2BodyId body, float width, float length) {
    b2Polygon polygon = b2MakeBox(width / 2, length / 2);
    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.enableHitEvents = true;
    if (onBridge) {
        shape_def.filter.categoryBits = CollisionCategories::CategoryBridgeObj;
        shape_def.filter.maskBits = CollisionCategories::CategoryBridgeCar;
    } else {
        shape_def.filter.categoryBits = CollisionCategories::CategoryGroundObj;
        shape_def.filter.maskBits = CollisionCategories::CategoryGroundCar;
    }
    b2ShapeId shape = b2CreatePolygonShape(body, &shape_def, &polygon);
    b2Shape_EnableContactEvents(shape, true);
}

StaticObject::StaticObject(b2WorldId world, b2Vec2 position, StaticObjectParam& params):
        onBridge(params.onBridge) {
    mass = params.mass;
    b2BodyDef bodyDef = initStaticObjectBodyDef(position);
    body = b2CreateBody(world, &bodyDef);

    setShape(body, params.width, params.length);
}

void StaticObject::onCollision([[maybe_unused]] Collidable* other,
                               [[maybe_unused]] float approachSpeed,
                               [[maybe_unused]] float deltaTime,
                               [[maybe_unused]] const b2Vec2& contactNormal) {}

float StaticObject::getMass() const { return mass; }

b2Rot StaticObject::getRotation(const b2Vec2& contactNormal) const {
    b2Vec2 normalized = b2Normalize(contactNormal);
    float angle = b2Atan2(normalized.y, normalized.x);
    return b2MakeRot(angle);
}

void StaticObject::applyCollision([[maybe_unused]] const CollisionInfo& info) {}

StaticObject::~StaticObject() {}
