#pragma once
#include <box2d/box2d.h>
#include <unordered_map>
#include <string>
#include <utility>
#include <iostream>

// -----------------------------------------------------
// Contact listener personalizado para detectar colisiones
// -----------------------------------------------------
class CollisionListener : public b2ContactListener {
public:
    void BeginContact(b2Contact* contact) override {
        const char* nameA = reinterpret_cast<const char*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
        const char* nameB = reinterpret_cast<const char*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

        std::cout << "¡Colisión detectada entre: "
                  << (nameA ? nameA : "desconocido")
                  << " y "
                  << (nameB ? nameB : "desconocido")
                  << "!" << std::endl;
    }
};

// -----------------------------------------------------
// Wrapper del motor físico
// -----------------------------------------------------
class PhysicsEngine {
    b2World world;
    CollisionListener collisionListener;
    std::unordered_map<std::string, b2Body*> bodies;

public:
    PhysicsEngine();

    void addCar(const std::string& id, float x, float y);
    void addWall(float x, float y, float width, float height);
    void applyImpulse(const std::string& id, float fx, float fy);
    void step(float deltaTime);
    std::pair<float, float> getPosition(const std::string& id);
};
