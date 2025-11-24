#include <vector>

#include <gtest/gtest.h>

#include "../client/cmd/client_to_server_cheat.h"

TEST(CTSProtocolSerializationTest, Cheat) {
    uint8_t cheatType = CHEAT_WIN;
    ClientToServerCheat cmd(cheatType);

    std::vector<uint8_t> bytes = cmd.to_bytes();

    ASSERT_EQ(bytes.size(), 2);
    EXPECT_EQ(bytes[0], CHEAT_COMMAND);
    EXPECT_EQ(bytes[1], cheatType);
}
