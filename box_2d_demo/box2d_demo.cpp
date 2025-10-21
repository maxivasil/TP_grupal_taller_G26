#include <box2d/box2d.h>
#include <iostream>
#include <iomanip>

int main() {
    std::cout << "=== DEMO BOX2D: Prueba de Choque ===" << std::endl;

    // 1. Crear el mundo físico (sin gravedad en eje Y)
    b2Vec2 gravity(0.0f, 0.0f);
    b2World world(gravity);

    // 2. Crear el auto (dinámico)
    b2BodyDef carDef;
    carDef.type = b2_dynamicBody;
    carDef.position.Set(0.0f, 0.0f);
    b2Body* car = world.CreateBody(&carDef);

    b2PolygonShape carShape;
    carShape.SetAsBox(1.0f, 0.5f); // tamaño del auto

    b2FixtureDef carFixture;
    carFixture.shape = &carShape;
    carFixture.density = 1.0f;
    carFixture.friction = 0.3f;
    car->CreateFixture(&carFixture);

    // 3. Crear un muro estático
    b2BodyDef wallDef;
    wallDef.position.Set(5.0f, 0.0f);
    b2Body* wall = world.CreateBody(&wallDef);

    b2PolygonShape wallShape;
    wallShape.SetAsBox(0.5f, 2.0f);
    wall->CreateFixture(&wallShape, 0.0f);

    // 4. Aplicar un impulso al auto hacia el muro
    car->ApplyLinearImpulseToCenter(b2Vec2(10.0f, 0.0f), true);

    // 5. Simular el mundo físico
    const float timeStep = 1.0f / 60.0f;
    const int velocityIterations = 6;
    const int positionIterations = 2;

    for (int i = 0; i < 60; ++i) {
        world.Step(timeStep, velocityIterations, positionIterations);

        b2Vec2 pos = car->GetPosition();
        b2Vec2 vel = car->GetLinearVelocity();

        std::cout << std::fixed << std::setprecision(2)
                  << "Paso " << std::setw(3) << i
                  << " | Pos(" << pos.x << ", " << pos.y << ")"
                  << " | Vel(" << vel.x << ", " << vel.y << ")" << std::endl;
    }

    std::cout << "=== Simulación finalizada ===" << std::endl;
    return 0;
}
