#include <exception>
#include <iostream>
#include <box2d/box2d.h>
#include <cstdint>
#include "car.h"
#include "session.h"
#include "physics:engine.h"
#include "static_object.h"
//int argc, char* argv[]
int main() {
    /*
    try {
        int ret = 1;
        const char* servname = NULL;
        if (argc != 2) {
            std::cerr << "Bad program call. Expected " << argv[0] << " <servicename>\n";
            return ret;
        }
        servname = argv[1];
        ServerSession session(servname);
        session.run();
        ret = 0;
        return ret;
    } catch (const std::exception& err) {
        std::cerr << "Something went wrong and an exception was caught: " << err.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Something went wrong and an unknown exception was caught.\n";
        return 1;
    }
    */

    PhysicsEngine physics;
    b2WorldId world = physics.getWorld();

    CarStats stats = {
        .acceleration = 20.0f,
        .max_speed = 100.0f,
        .turn_speed = 5.0f,
        .mass = 1200.0f,
        .brake_force = 15.0f,
        .handling = 0.8f,
        .health_max = 100.0f,
        .length = 2.0f,
        .width = 4.0f
    };

    StaticObjectParam wall_params = {
        .length = 10.0f,
        .width = 10.0f,
        .mass = 10000.0f
    };

    float p = 20.0f;
    Car carA(world, std::move(stats), {-p, 0.0f}, b2MakeRot(0));
    StaticObject wall(world, b2Vec2_zero, wall_params);
    Car carB(world, std::move(stats), { p, 0.0f}, b2MakeRot(B2_PI));

    const float timeStep = 1.0f / 60.0f;

    for (int i = 0; i < 180; ++i) {
        CarInput inputA = {true, false, Direction::FORWARD};
        CarInput inputB = {true, false, Direction::FORWARD};

        carA.updatePhysics(inputA);
        carB.updatePhysics(inputB);

        physics.step(timeStep, 4);

        b2Vec2 posA = carA.getPosition();
        b2Vec2 posB = carB.getPosition();

        std::cout << "Step " << i << ":\n";
        std::cout << "  CarA Pos (" << posA.x << ", " << posA.y << ") HP=" << carA.getCurrentHealth() << "\n";
        std::cout << "  CarB Pos (" << posB.x << ", " << posB.y << ") HP=" << carB.getCurrentHealth() << "\n\n";
    }

    return 0;
}
