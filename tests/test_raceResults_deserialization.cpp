#include <cstring>
#include <vector>

#include <arpa/inet.h>
#include <gtest/gtest.h>

#include "../common/buffer_utils.h"

#include "testable_server_to_client.h"

TEST(STCProtocolDeserializationTest, RaceResults) {
    std::vector<uint8_t> bytes;
    BufferUtils::append_uint8(bytes, SERVER_TO_CLIENT_RACE_RESULTS);
    BufferUtils::append_uint8(bytes, 1);
    BufferUtils::append_uint8(bytes, 2);

    auto appendPlayer = [&](uint32_t playerId, const std::string& name, float time, uint8_t pos) {
        BufferUtils::append_uint32(bytes, playerId);

        BufferUtils::append_uint16(bytes, static_cast<uint16_t>(name.size()));

        BufferUtils::append_bytes(bytes, name.data(), name.size());

        BufferUtils::append_float(bytes, time);

        BufferUtils::append_uint8(bytes, pos);
    };

    appendPlayer(1, "Maxi", 63.789f, 1);
    appendPlayer(2, "Lara", 70.001f, 2);

    auto cmd = ServerToClientRaceResults::from_bytes(bytes);

    ASSERT_EQ(cmd.results.size(), 2);
    EXPECT_TRUE(cmd.isFinished);

    const auto& r0 = cmd.results[0];
    EXPECT_EQ(r0.playerId, 1);
    EXPECT_EQ(r0.playerName, "Maxi");
    EXPECT_FLOAT_EQ(r0.finishTime, 63.789f);
    EXPECT_EQ(r0.position, 1);

    const auto& r1 = cmd.results[1];
    EXPECT_EQ(r1.playerId, 2);
    EXPECT_EQ(r1.playerName, "Lara");
    EXPECT_FLOAT_EQ(r1.finishTime, 70.001f);
    EXPECT_EQ(r1.position, 2);
}
