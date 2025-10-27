#ifndef WORLD_CONTACT_LISTENER_H
#define WORLD_CONTACT_LISTENER_H

#include <box2d/box2d.h>
#include "car.h"

class WorldContactListener {
private:
    void processCarCollision(Car& carA, Car& carB, float approachSpeed, float deltaTime);
    
    float getImpactForce(Car& carA, Car& carB, float approachSpeed, float deltaTime);
    
    float getImpactAngle(Car& carA, Car& carB);

public:
    WorldContactListener();

    void BeginContact(b2ContactHitEvent* contact, float deltaTime);

    ~WorldContactListener();
};


#endif
