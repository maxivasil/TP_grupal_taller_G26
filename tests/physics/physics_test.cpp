#include <gtest/gtest.h>
#include <box2d/box2d.h>
#include "server/PhysicsEngine.h"

TEST(PhysicsEngineTest, WorldCreationWorks) {
    PhysicsEngine physics;
    b2WorldId world = physics.getWorld();
    EXPECT_TRUE(b2World_IsValid(world));
}

TEST(PhysicsEngineTest, StepSimulationDoesNotCrash) {
    PhysicsEngine physics;
    EXPECT_NO_THROW(physics.step(1.0f/60.0f, 4));
}

TEST(PhysicsEngineTest, GravityIsZeroForTopDownRacing) {
    PhysicsEngine physics;
    b2WorldId world = physics.getWorld();
    b2Vec2 gravity = b2World_GetGravity(world);
    EXPECT_FLOAT_EQ(gravity.x, 0.0f);
    EXPECT_FLOAT_EQ(gravity.y, 0.0f);
}