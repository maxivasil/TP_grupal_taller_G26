#include "CheckpointManager.h"

#include <memory>
#include <utility>

#include "CollisionCategories.h"
#include "SensorData.h"

CheckpointManager::CheckpointManager(): nextCheckpointId(0) {}

b2BodyDef CheckpointManager::initCheckpointBodyDef(b2Vec2 position) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.position = position;
    bodyDef.isAwake = true;
    bodyDef.type = b2_kinematicBody;
    return bodyDef;
}

void CheckpointManager::setShape(b2BodyId body, float width, float length,
                                 PhysicalCheckpoint& ckpt) {
    b2Polygon box = b2MakeBox(width / 2, length / 2);
    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.isSensor = true;
    shape_def.enableSensorEvents = true;
    shape_def.filter.categoryBits = CollisionCategories::CategoryCheckpoint;
    shape_def.filter.maskBits = CollisionCategories::CategoryAll;
    shape_def.filter.groupIndex = 0;
    ckpt.sensorData = std::make_unique<SensorData>(SensorType::Checkpoint, ckpt.id);
    shape_def.userData = ckpt.sensorData.get();
    b2CreatePolygonShape(body, &shape_def, &box);
}

void CheckpointManager::createCheckpoint(b2WorldId world, b2Vec2 position, float width,
                                         float length) {
    b2BodyDef bodyDef = initCheckpointBodyDef(position);
    b2BodyId body = b2CreateBody(world, &bodyDef);

    PhysicalCheckpoint ckpt;
    ckpt.id = nextCheckpointId;
    ckpt.body = body;
    ckpt.position = position;

    setShape(body, width, length, ckpt);
    checkpoints.push_back(std::move(ckpt));
    nextCheckpointId++;
}

bool CheckpointManager::hasCarFinished(Car* car) const {
    auto it = lastCheckpointPassed.find(car);
    if (it != lastCheckpointPassed.end())
        return nextCheckpointId - 1 == it->second;
    return false;
}

void CheckpointManager::onCarEnterCheckpoint(Car* car, int checkpointId) {
    auto it = lastCheckpointPassed.find(car);
    int last = (it == lastCheckpointPassed.end()) ? -1 : it->second;

    if (last + 1 == checkpointId) {
        lastCheckpointPassed[car] = checkpointId;
    }
}

CheckpointManager::~CheckpointManager() {}
