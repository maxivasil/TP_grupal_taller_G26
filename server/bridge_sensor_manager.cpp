#include "bridge_sensor_manager.h"

#include <utility>

#include "collision_categories.h"

BridgeSensorManager::BridgeSensorManager() {}

b2BodyDef BridgeSensorManager::initBodyDef(b2Vec2 position) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.position = position;
    bodyDef.isAwake = true;
    bodyDef.type = b2_kinematicBody;
    return bodyDef;
}

void BridgeSensorManager::setShape(b2BodyId body, float width, float length,
                                   PhysicalBridgeSensor& bridge) {
    b2Polygon box = b2MakeBox(width / 2, length / 2);
    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.isSensor = true;
    shape_def.enableSensorEvents = true;
    shape_def.filter.categoryBits = CollisionCategories::CategoryBridgeSensor;
    shape_def.filter.maskBits = CollisionCategories::CategoryAll;
    shape_def.filter.groupIndex = 0;
    bridge.sensorData = std::make_unique<SensorData>(SensorType::BridgeLevel, bridge.id);
    shape_def.userData = bridge.sensorData.get();
    b2CreatePolygonShape(body, &shape_def, &box);
}

void BridgeSensorManager::createBridgeSensor(b2WorldId world, b2Vec2 position, float width,
                                             float length) {
    b2BodyDef bodyDef = initBodyDef(position);
    b2BodyId body = b2CreateBody(world, &bodyDef);

    PhysicalBridgeSensor ckpt;
    ckpt.id = 0;
    ckpt.body = body;
    ckpt.position = position;

    setShape(body, width, length, ckpt);
    sensors.push_back(std::move(ckpt));
}

BridgeSensorManager::~BridgeSensorManager() {}
