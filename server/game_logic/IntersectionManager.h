#ifndef INTERSECTION_MANAGER_H
#define INTERSECTION_MANAGER_H

#include <list>
#include <memory>

#include <box2d/box2d.h>

#include "SensorData.h"

struct PhysicalIntersection {
    int id;
    b2BodyId body;
    b2Vec2 position;
    bool up;
    bool down;
    bool right;
    bool left;
    std::unique_ptr<SensorData> sensorData;
};

class IntersectionManager {
private:
    std::list<PhysicalIntersection> sensors;

    b2BodyDef initBodyDef(b2Vec2 position);

    void setShape(b2BodyId body, float width, float length, PhysicalIntersection& intersection);

public:
    IntersectionManager();
    ~IntersectionManager();

    void createIntersection(b2WorldId world, b2Vec2 position, float width, float height, bool up,
                            bool down, bool right, bool left);
};

#endif
