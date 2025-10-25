#include "physics.h"

PhysicsEngine::PhysicsEngine() : world(b2Vec2(0.0f, 0.0f)) {
    world.SetContactListener(&collisionListener);
}

void PhysicsEngine::addCar(const std::string& id, float x, float y) {
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position.Set(x, y);
    b2Body* body = world.CreateBody(&def);

    b2PolygonShape shape;
    shape.SetAsBox(1.0f, 0.5f);

    b2FixtureDef fixture;
    fixture.shape = &shape;
    fixture.density = 1.0f;
    fixture.friction = 0.3f;
    body->CreateFixture(&fixture);

    // Guardar datos del auto
    CarData car{id, 100.0f, body};
    cars[id] = car;

    // Vincular el puntero de CarData al cuerpo (para el listener)
    body->GetUserData().pointer = reinterpret_cast<uintptr_t>(&cars[id]);
}

void PhysicsEngine::addWall(float x, float y, float w, float h) {
    b2BodyDef def;
    def.position.Set(x, y);
    b2Body* body = world.CreateBody(&def);

    b2PolygonShape shape;
    shape.SetAsBox(w / 2, h / 2);
    body->CreateFixture(&shape, 0.0f);

    walls.push_back(body);
}

void PhysicsEngine::applyImpulse(const std::string& id, float fx, float fy) {
    auto it = cars.find(id);
    if (it != cars.end())
        it->second.body->ApplyLinearImpulseToCenter(b2Vec2(fx, fy), true);
}

void PhysicsEngine::step(float deltaTime) {
    world.Step(deltaTime, 6, 2);
}

std::pair<float, float> PhysicsEngine::getPosition(const std::string& id) {
    auto it = cars.find(id);
    if (it == cars.end()) return {0, 0};
    b2Vec2 pos = it->second.body->GetPosition();
    return {pos.x, pos.y};
}

float PhysicsEngine::getHealth(const std::string& id) {
    auto it = cars.find(id);
    if (it == cars.end()) return 0;
    return it->second.health;
}
