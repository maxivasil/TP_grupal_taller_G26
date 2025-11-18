#ifndef CHECKPOINT_MANAGER_H
#define CHECKPOINT_MANAGER_H

#include <list>
#include <unordered_map>

#include <box2d/box2d.h>

#include "Car.h"
#include "PhysicalCheckpoint.h"

class CheckpointManager {
private:
    std::list<PhysicalCheckpoint> checkpoints;
    std::unordered_map<Car*, int> lastCheckpointPassed;
    int nextCheckpointId;

    b2BodyDef initCheckpointBodyDef(b2Vec2 position);

    void setShape(b2BodyId body, float width, float length, PhysicalCheckpoint& ckpt);

public:
    CheckpointManager();

    void createCheckpoint(b2WorldId world, b2Vec2 position, float width, float length);

    bool hasCarFinished(Car* car) const;

    void onCarEnterCheckpoint(Car* car, int checkpointId);

    ~CheckpointManager();
};


#endif
