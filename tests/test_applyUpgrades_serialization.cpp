#include <cstring>
#include <vector>

#include <arpa/inet.h>
#include <gtest/gtest.h>

#include "../client/cmd/client_to_server_applyUpgrades_client.h"
#include "../common/buffer_utils.h"


TEST(CTSProtocolSerializationTest, ApplyUpgrades) {
    CarUpgrades upgrades;
    upgrades.acceleration_boost = 1.5f;
    upgrades.speed_boost = 2.7f;
    upgrades.handling_boost = 0.8f;
    upgrades.health_boost = 10.0f;

    ClientToServerApplyUpgrades_Client cmd(upgrades);
    std::vector<uint8_t> bytes = cmd.to_bytes();

    ASSERT_EQ(bytes[0], APPLY_UPGRADES_COMMAND);

    size_t offset = 1;

    auto readFloat = [&](float original) {
        float value;
        BufferUtils::read_float(bytes, offset, value);
        EXPECT_FLOAT_EQ(value, original);
    };

    readFloat(upgrades.acceleration_boost);
    readFloat(upgrades.speed_boost);
    readFloat(upgrades.handling_boost);
    readFloat(upgrades.health_boost);

    EXPECT_EQ(offset, bytes.size());
}
