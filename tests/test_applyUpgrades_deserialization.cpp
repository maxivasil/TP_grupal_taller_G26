#include <cstring>
#include <vector>

#include <arpa/inet.h>
#include <gtest/gtest.h>

#include "../common/buffer_utils.h"
#include "../server/game_logic/CarUpgrades.h"

#include "testable_client_to_server.h"

TEST(CTSProtocolDeserializationTest, ApplyUpgrades) {
    std::vector<uint8_t> bytes;
    BufferUtils::append_uint8(bytes, APPLY_UPGRADES_COMMAND);

    float accel = 1.5f, speed = 2.7f, handling = 0.8f, health = 10.0f;
    BufferUtils::append_float(bytes, accel);
    BufferUtils::append_float(bytes, speed);
    BufferUtils::append_float(bytes, handling);
    BufferUtils::append_float(bytes, health);

    uint32_t dummyClientId = 42;
    ClientToServerApplyUpgrades* cmd =
            ClientToServerApplyUpgrades::from_bytes(bytes, dummyClientId);

    ASSERT_NE(cmd, nullptr);

    const CarUpgrades& res = cmd->upgrades;
    EXPECT_FLOAT_EQ(res.acceleration_boost, accel);
    EXPECT_FLOAT_EQ(res.speed_boost, speed);
    EXPECT_FLOAT_EQ(res.handling_boost, handling);
    EXPECT_FLOAT_EQ(res.health_boost, health);

    delete cmd;
}
