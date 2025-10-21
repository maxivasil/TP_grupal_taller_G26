#include "physics.h"

PhysicsEngine::PhysicsEngine() : world(b2Vec2(0.0f, 0.0f)) {}

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

    bodies[id] = body;
}

void PhysicsEngine::addWall(float x, float y, float w, float h) {
    b2BodyDef def;
    def.position.Set(x, y);
    b2Body* body = world.CreateBody(&def);
    b2PolygonShape shape;
    shape.SetAsBox(w / 2, h / 2);
    body->CreateFixture(&shape, 0.0f);
}

void PhysicsEngine::applyImpulse(const std::string& id, float fx, float fy) {
    auto it = bodies.find(id);
    if (it != bodies.end())
        it->second->ApplyLinearImpulseToCenter(b2Vec2(fx, fy), true);
}

void PhysicsEngine::step(float deltaTime) {
    world.Step(deltaTime, 6, 2);
}

std::pair<float, float> PhysicsEngine::getPosition(const std::string& id) {
    auto it = bodies.find(id);
    if (it == bodies.end()) return {0, 0};
    b2Vec2 pos = it->second->GetPosition();
    return {pos.x, pos.y};
}
