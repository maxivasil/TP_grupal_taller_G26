#pragma once
#include <box2d/box2d.h>
#include <unordered_map>
#include <string>

class PhysicsEngine {
    b2World world;
    std::unordered_map<std::string, b2Body*> bodies;

public:
    PhysicsEngine();

    void addCar(const std::string& id, float x, float y);
    void addWall(float x, float y, float width, float height);
    void applyImpulse(const std::string& id, float fx, float fy);
    void step(float deltaTime);
    std::pair<float, float> getPosition(const std::string& id);
};
