#include <vector>

#include <gtest/gtest.h>

#include "testable_client_to_server.h"

TEST(CTSProtocolDeserializationTest, Move) {
    uint8_t direction = MOVE_RIGHT;
    uint32_t clientId = 42;

    std::vector<uint8_t> bytes = {MOVE_COMMAND, direction};

    ClientToServerMove* cmd = ClientToServerMove::from_bytes(bytes, clientId);

    ASSERT_NE(cmd, nullptr);
    EXPECT_EQ(cmd->direction, direction);

    delete cmd;
}
