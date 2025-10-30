#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include <memory>

#include <box2d/box2d.h>

#include "sensor_data.h"

struct PhysicalCheckpoint {
    int id;
    b2BodyId body;
    b2Vec2 position;
    std::unique_ptr<SensorData> sensorData;
};


#endif
