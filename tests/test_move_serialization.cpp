#include <vector>

#include <gtest/gtest.h>

#include "../client/cmd/client_to_server_move_client.h"

TEST(CTSProtocolSerializationTest, Move) {
    uint8_t direction = MOVE_LEFT;
    ClientToServerMove_Client cmd(direction);

    std::vector<uint8_t> bytes = cmd.to_bytes();

    ASSERT_EQ(bytes.size(), 2);
    EXPECT_EQ(bytes[0], MOVE_COMMAND);
    EXPECT_EQ(bytes[1], direction);
}
