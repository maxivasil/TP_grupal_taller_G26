#include "CheckpointManager.h"

#include "CollisionCategories.h"

CheckpointManager::CheckpointManager(): nextCheckpointId(0) {}

b2BodyDef CheckpointManager::initCheckpointBodyDef(b2Vec2 position) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.position = position;
    bodyDef.isAwake = true;
    bodyDef.type = b2_kinematicBody;
    return bodyDef;
}

void CheckpointManager::setShape(b2BodyId body, float width, float length, int id) {
    b2Polygon box = b2MakeBox(width / 2, length / 2);
    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.isSensor = true;
    shape_def.enableSensorEvents = true;
    shape_def.filter.categoryBits = CollisionCategories::CategorySensor;
    shape_def.filter.maskBits = CollisionCategories::CategoryDynamic;
    shape_def.filter.groupIndex = 0;
    shape_def.userData = reinterpret_cast<void*>(static_cast<intptr_t>(id));
    b2CreatePolygonShape(body, &shape_def, &box);
}

void CheckpointManager::createCheckpoint(b2WorldId world, b2Vec2 position, float width,
                                         float length) {
    b2BodyDef bodyDef = initCheckpointBodyDef(position);
    b2BodyId body = b2CreateBody(world, &bodyDef);
    setShape(body, width, length, nextCheckpointId);
    checkpoints.push_back({nextCheckpointId, body, position});
    nextCheckpointId++;
}

bool CheckpointManager::hasCarFinished(Car& car) const {
    auto it = lastCheckpointPassed.find(&car);
    if (it != lastCheckpointPassed.end())
        return nextCheckpointId - 1 == it->second;
    return false;
}

void CheckpointManager::onCarEnterCheckpoint(Car* car, int checkpointId) {
    int last = lastCheckpointPassed[car];
    if (last + 1 == checkpointId) {
        lastCheckpointPassed[car] = checkpointId;
    }
}

CheckpointManager::~CheckpointManager() {}
