#include "IntersectionManager.h"

#include <utility>

#include "CollisionCategories.h"

IntersectionManager::IntersectionManager() {}

b2BodyDef IntersectionManager::initBodyDef(b2Vec2 position) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.position = position;
    bodyDef.isAwake = true;
    bodyDef.type = b2_kinematicBody;
    return bodyDef;
}

void IntersectionManager::setShape(b2BodyId body, float width, float length,
                                   PhysicalIntersection& intersection) {
    b2Polygon box = b2MakeBox(width / 2, length / 2);
    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.isSensor = true;
    shape_def.enableSensorEvents = true;

    shape_def.filter.categoryBits = CollisionCategories::CategoryIntersection;
    shape_def.filter.maskBits = CollisionCategories::CategoryAll;
    shape_def.filter.groupIndex = 0;

    int dirMask = 0;
    if (intersection.up)
        dirMask |= DirectionMask::DIR_UP;
    if (intersection.down)
        dirMask |= DirectionMask::DIR_DOWN;
    if (intersection.right)
        dirMask |= DirectionMask::DIR_RIGHT;
    if (intersection.left)
        dirMask |= DirectionMask::DIR_LEFT;

    intersection.sensorData = std::make_unique<SensorData>(SensorType::Intersection, dirMask);

    shape_def.userData = intersection.sensorData.get();
    b2CreatePolygonShape(body, &shape_def, &box);
}

void IntersectionManager::createIntersection(b2WorldId world, b2Vec2 position, float width,
                                             float length, bool up, bool down, bool right,
                                             bool left) {
    b2BodyDef bodyDef = initBodyDef(position);
    b2BodyId body = b2CreateBody(world, &bodyDef);

    PhysicalIntersection inter;
    inter.id = 0;
    inter.body = body;
    inter.position = position;
    inter.up = up;
    inter.down = down;
    inter.right = right;
    inter.left = left;

    setShape(body, width, length, inter);
    sensors.push_back(std::move(inter));
}

IntersectionManager::~IntersectionManager() {}
