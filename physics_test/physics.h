#pragma once
#include <box2d/box2d.h>
#include <unordered_map>
#include <string>
#include <utility>
#include <iostream>
#include <vector>

// -----------------------------------------------------
// Datos del auto controlado
// -----------------------------------------------------
struct CarData {
    std::string id;
    float health;      // Salud del auto (0 a 100)
    b2Body* body;
};

// -----------------------------------------------------
// Listener de colisiones personalizado
// -----------------------------------------------------
class CollisionListener : public b2ContactListener {
public:
    void BeginContact(b2Contact* contact) override {
        b2Body* bodyA = contact->GetFixtureA()->GetBody();
        b2Body* bodyB = contact->GetFixtureB()->GetBody();

        CarData* carA = reinterpret_cast<CarData*>(bodyA->GetUserData().pointer);
        CarData* carB = reinterpret_cast<CarData*>(bodyB->GetUserData().pointer);

        // Detectar si alguno es un auto
        if (carA && carA->health > 0) {
            float damage = computeDamage(bodyA, bodyB);
            carA->health = std::max(0.0f, carA->health - damage);
            std::cout << "💥 " << carA->id << " recibió daño: -" << damage
                      << " (Salud restante: " << carA->health << ")\n";
            reduceVelocity(bodyA, 0.6f);
        }

        if (carB && carB->health > 0) {
            float damage = computeDamage(bodyB, bodyA);
            carB->health = std::max(0.0f, carB->health - damage);
            std::cout << "💥 " << carB->id << " recibió daño: -" << damage
                      << " (Salud restante: " << carB->health << ")\n";
            reduceVelocity(bodyB, 0.6f);
        }
    }

private:
    static float computeDamage(b2Body* a, b2Body* b) {
        b2Vec2 va = a->GetLinearVelocity();
        b2Vec2 vb = b->GetLinearVelocity();
        float impact = (va - vb).Length();
        return std::min(impact * 5.0f, 30.0f);  // Escala de daño
    }

    static void reduceVelocity(b2Body* body, float factor) {
        b2Vec2 v = body->GetLinearVelocity();
        body->SetLinearVelocity(b2Vec2(v.x * factor, v.y * factor));
    }
};

// -----------------------------------------------------
// Wrapper del motor físico
// -----------------------------------------------------
class PhysicsEngine {
    b2World world;
    CollisionListener collisionListener;
    std::unordered_map<std::string, CarData> cars;
    std::vector<b2Body*> walls;

public:
    PhysicsEngine();

    void addCar(const std::string& id, float x, float y);
    void addWall(float x, float y, float width, float height);
    void applyImpulse(const std::string& id, float fx, float fy);
    void step(float deltaTime);
    std::pair<float, float> getPosition(const std::string& id);
    float getHealth(const std::string& id);
};
