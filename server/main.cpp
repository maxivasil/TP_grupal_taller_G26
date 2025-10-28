#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>

#include <box2d/box2d.h>

#include "Car.h"
#include "CheckpointManager.h"
#include "PhysicsEngine.h"
#include "Player.h"
#include "StaticObject.h"
#include "session.h"

// int argc, char* argv[]
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

    CheckpointManager checkpointManager;
    PhysicsEngine physics(checkpointManager);
    b2WorldId world = physics.getWorld();
    checkpointManager.createCheckpoint(world, b2Vec2{-10.0f, 0.0f}, 6.0f, 6.0f);
    checkpointManager.createCheckpoint(world, b2Vec2_zero, 6.0f, 6.0f);

    CarStats statsA = {.acceleration = 20.0f,
                       .max_speed = 100.0f,
                       .turn_speed = 5.0f,
                       .mass = 1200.0f,
                       .brake_force = 15.0f,
                       .handling = 0.8f,
                       .health_max = 100.0f,
                       .length = 2.0f,
                       .width = 4.0f};

    CarStats statsB = {.acceleration = 20.0f,
                       .max_speed = 100.0f,
                       .turn_speed = 5.0f,
                       .mass = 1200.0f,
                       .brake_force = 15.0f,
                       .handling = 0.8f,
                       .health_max = 100.0f,
                       .length = 2.0f,
                       .width = 4.0f};

    StaticObjectParam wall_params = {.length = 3.0f, .width = 3.0f, .mass = 10000.0f};

    float p = 20.0f;
    std::unique_ptr<Car> carA =
            std::make_unique<Car>(world, std::move(statsA), b2Vec2{-p, 0.0f}, b2MakeRot(0));
    StaticObject wall(world, b2Vec2_zero, wall_params);
    std::unique_ptr<Car> carB =
            std::make_unique<Car>(world, std::move(statsB), b2Vec2{p, 0.0f}, b2MakeRot(B2_PI));

    Player playerA("A");
    Player playerB("B");

    playerA.assignCar(std::move(carA));
    playerB.assignCar(std::move(carB));

    const float timeStep = 1.0f / 60.0f;

    for (int i = 0; i < 180; ++i) {
        playerA.accelerate();
        playerB.accelerate();

        physics.step(timeStep, 4);

        b2Vec2 posA = playerA.getPosition();
        b2Vec2 posB = playerB.getPosition();

        std::cout << "Step " << i << ":\n";
        std::cout << "  CarA Pos (" << posA.x << ", " << posA.y
                  << ") HP=" << playerA.getCurrentHealth() << "\n";
        std::cout << "  CarB Pos (" << posB.x << ", " << posB.y
                  << ") HP=" << playerB.getCurrentHealth() << "\n\n";
    }

    std::cout << (checkpointManager.hasCarFinished(*playerA.getCar()) ? "Termino" : "No termino")
              << std::endl;
    std::cout << (checkpointManager.hasCarFinished(*playerB.getCar()) ? "Termino" : "No termino")
              << std::endl;

    return 0;
}
