#include <box2d/box2d.h>
#include <iostream>

int main() {
    // Crear el mundo (sin gravedad en Y, ya que es un juego de autos 2D)
    b2Vec2 gravity(0.0f, 0.0f);
    b2World world(gravity);

    // Crear el auto (cuerpo dinámico)
    b2BodyDef carDef;
    carDef.type = b2_dynamicBody;
    carDef.position.Set(0.0f, 0.0f);
    b2Body* car = world.CreateBody(&carDef);

    b2PolygonShape carShape;
    carShape.SetAsBox(1.0f, 0.5f); // ancho x alto

    b2FixtureDef carFixture;
    carFixture.shape = &carShape;
    carFixture.density = 1.0f;
    carFixture.friction = 0.3f;
    car->CreateFixture(&carFixture);

    // Crear un muro (cuerpo estático)
    b2BodyDef wallDef;
    wallDef.position.Set(5.0f, 0.0f);
    b2Body* wall = world.CreateBody(&wallDef);

    b2PolygonShape wallShape;
    wallShape.SetAsBox(0.5f, 2.0f);
    wall->CreateFixture(&wallShape, 0.0f);

    // Aplicar una fuerza al auto hacia el muro
    car->ApplyLinearImpulseToCenter(b2Vec2(10.0f, 0.0f), true);

    // Simulación paso a paso
    const float timeStep = 1.0f / 60.0f;
    const int velocityIterations = 6;
    const int positionIterations = 2;

    for (int i = 0; i < 180; ++i) {
        world.Step(timeStep, velocityIterations, positionIterations);
        b2Vec2 pos = car->GetPosition();
        float angle = car->GetAngle();
        std::cout << "Paso " << i
                  << ": Posición (" << pos.x << ", " << pos.y
                  << ") Ángulo: " << angle << std::endl;
    }

    return 0;
}
