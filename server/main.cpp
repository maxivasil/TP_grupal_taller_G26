#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>

#include <box2d/box2d.h>
#include <cstdint>
#include "car.h"
#include "bridge_sensor_manager.h"
#include "checkpoint_manager.h"
#include "city.h"
#include "track.h"

#include "session.h"
#include "physics_engine.h"
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

    std::string trackFile = "tracks/track.yaml";
    Track track(trackFile);
    City city(CityName::LibertyCity);
    CheckpointManager checkpointManager;
    BridgeSensorManager bridgeSensorManager;
    PhysicsEngine physics(checkpointManager);
    b2WorldId world = physics.getWorld();
    for (auto ckpt: track.getCheckpoints()) {
        checkpointManager.createCheckpoint(world, b2Vec2{ckpt.x, ckpt.y}, ckpt.width, ckpt.height);
    }
    std::list<StaticObject> staticObjects;
    for (auto staticObj: city.getStaticObjects()) {
        StaticObjectParam params = {.length = staticObj.height,
                                    .width = staticObj.width,
                                    .mass = 10000.0f,
                                    .onBridge = staticObj.isUp};
        staticObjects.emplace_back(world, b2Vec2{staticObj.x, staticObj.y}, params);
    }
    for (auto bridgeSensor: city.getBridgeSensors()) {
        bridgeSensorManager.createBridgeSensor(world, b2Vec2{bridgeSensor.x, bridgeSensor.y},
                                               bridgeSensor.width, bridgeSensor.height);
    }

    CarStats statsA = {.acceleration = 20.0f,
                       .max_speed = 100.0f,
                       .turn_speed = 5.0f,
                       .mass = 1200.0f,
                       .brake_force = 15.0f,
                       .handling = 0.8f,
                       .health_max = 100.0f,
                       .length = 2,
                       .width = 1.5};

    CarStats statsB = {.acceleration = 20.0f,
                       .max_speed = 100.0f,
                       .turn_speed = 5.0f,
                       .mass = 1200.0f,
                       .brake_force = 15.0f,
                       .handling = 0.8f,
                       .health_max = 100.0f,
                       .length = 2,
                       .width = 1.5};

    std::unique_ptr<Car> carA =
            std::make_unique<Car>(world, std::move(statsA), b2Vec2{57.3, 308.6}, b2MakeRot(0));
    std::unique_ptr<Car> carB =
            std::make_unique<Car>(world, std::move(statsB), b2Vec2{85.1, 308.6}, b2MakeRot(B2_PI));

    Player playerA("A");
    Player playerB("B");

    playerA.assignCar(std::move(carA));
    playerB.assignCar(std::move(carB));

    const float timeStep = 1.0f / 60.0f;
    // 400 2400  -- ----- 57.3, 308.6
    // 580 2400  -------- 83.1, 308.6

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
