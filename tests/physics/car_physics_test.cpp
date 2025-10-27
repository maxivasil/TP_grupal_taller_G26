#include <gtest/gtest.h>
#include "server/PhysicsEngine.h"
#include "server/Car.h"

class CarPhysicsTest : public ::testing::Test {
protected:
    PhysicsEngine* physics;
    Car* car;
    
    void SetUp() override {
        physics = new PhysicsEngine();
        
        CarStats stats = {
            .acceleration = 20.0f,
            .max_speed = 50.0f,
            .turn_speed = 5.0f,
            .mass = 1200.0f,
            .brake_force = 15.0f,
            .handling = 0.8f,
            .health_max = 100.0f,
            .length = 4.0f,
            .width = 2.0f
        };
        
        car = new Car(physics->getWorld(), stats, {0.0f, 0.0f}, b2MakeRot(0));
    }
    
    void TearDown() override {
        delete car;
        delete physics;
    }
};

TEST_F(CarPhysicsTest, StartsAtOrigin) {
    b2Vec2 pos = car->getPosition();
    EXPECT_FLOAT_EQ(pos.x, 0.0f);
    EXPECT_FLOAT_EQ(pos.y, 0.0f);
}

TEST_F(CarPhysicsTest, StartsWithFullHealth) {
    EXPECT_FLOAT_EQ(car->getCurrentHealth(), 100.0f);
}

TEST_F(CarPhysicsTest, AccelerationIncreasesSpeed) {
    CarInput input = {true, false, Direction::FORWARD};
    
    float initialSpeed = b2Length(car->getLinearVelocity());
    
    // Simulate 1 second of acceleration
    for (int i = 0; i < 60; ++i) {
        car->updatePhysics(input);
        physics->step(1.0f/60.0f, 4);
    }
    
    float finalSpeed = b2Length(car->getLinearVelocity());
    EXPECT_GT(finalSpeed, initialSpeed);
}

TEST_F(CarPhysicsTest, MaxSpeedIsEnforced) {
    CarInput input = {true, false, Direction::FORWARD};
    
    // Simulate 5 seconds of full acceleration
    for (int i = 0; i < 300; ++i) {
        car->updatePhysics(input);
        physics->step(1.0f/60.0f, 4);
    }
    
    float speed = b2Length(car->getLinearVelocity());
    EXPECT_LE(speed, 50.5f);  // Max speed + small tolerance
}

TEST_F(CarPhysicsTest, BrakingSlowsDownCar) {
    // First accelerate
    CarInput accelInput = {true, false, Direction::FORWARD};
    for (int i = 0; i < 60; ++i) {
        car->updatePhysics(accelInput);
        physics->step(1.0f/60.0f, 4);
    }
    
    float speedBeforeBraking = b2Length(car->getLinearVelocity());
    
    // Now brake
    CarInput brakeInput = {false, true, Direction::FORWARD};
    for (int i = 0; i < 30; ++i) {
        car->updatePhysics(brakeInput);
        physics->step(1.0f/60.0f, 4);
    }
    
    float speedAfterBraking = b2Length(car->getLinearVelocity());
    EXPECT_LT(speedAfterBraking, speedBeforeBraking);
}

TEST_F(CarPhysicsTest, TurningChangesRotation) {
    // Need speed to turn
    CarInput accelInput = {true, false, Direction::FORWARD};
    for (int i = 0; i < 30; ++i) {
        car->updatePhysics(accelInput);
        physics->step(1.0f/60.0f, 4);
    }
    
    b2Rot initialRot = car->getRotation();
    
    // Turn left while moving
    CarInput turnInput = {true, false, Direction::LEFT};
    for (int i = 0; i < 30; ++i) {
        car->updatePhysics(turnInput);
        physics->step(1.0f/60.0f, 4);
    }
    
    b2Rot finalRot = car->getRotation();
    EXPECT_NE(initialRot.c, finalRot.c);  // Rotation changed
}

TEST_F(CarPhysicsTest, RepairRestoresFullHealth) {
    // Damage the car manually
    CollisionInfo info = {1000.0f, 0.0f};
    car->applyCollision(info);
    
    EXPECT_LT(car->getCurrentHealth(), 100.0f);
    
    car->repair();
    EXPECT_FLOAT_EQ(car->getCurrentHealth(), 100.0f);
}