#ifndef CHECKPOINT_MANAGER_H
#define CHECKPOINT_MANAGER_H

#include <unordered_map>
#include <vector>

#include <box2d/box2d.h>

#include "car.h"
#include "physical_checkpoint.h"

class CheckpointManager {
private:
    std::vector<PhysicalCheckpoint> checkpoints;
    std::unordered_map<Car*, int> lastCheckpointPassed;
    int nextCheckpointId;

    b2BodyDef initCheckpointBodyDef(b2Vec2 position);

    void setShape(b2BodyId body, float width, float length, int id);

public:
    CheckpointManager();

    void createCheckpoint(b2WorldId world, b2Vec2 position, float width, float length);

    bool hasCarFinished(Car& car) const;

    void onCarEnterCheckpoint(Car* car, int checkpointId);

    ~CheckpointManager();
};


#endif
