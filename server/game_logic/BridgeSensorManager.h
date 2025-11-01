#ifndef BRIDGE_SENSOR_MANAGER_H
#define BRIDGE_SENSOR_MANAGER_H

#include <list>
#include <memory>

#include <box2d/box2d.h>

#include "SensorData.h"

struct PhysicalBridgeSensor {
    int id;
    b2BodyId body;
    b2Vec2 position;
    std::unique_ptr<SensorData> sensorData;
};

class BridgeSensorManager {
private:
    std::list<PhysicalBridgeSensor> sensors;

    b2BodyDef initBodyDef(b2Vec2 position);

    void setShape(b2BodyId body, float width, float length, PhysicalBridgeSensor& ckpt);

public:
    BridgeSensorManager();
    ~BridgeSensorManager();

    void createBridgeSensor(b2WorldId world, b2Vec2 position, float width, float height);
};


#endif
