#include <vector>

#include <gtest/gtest.h>

#include "testable_client_to_server.h"

TEST(CTSProtocolDeserializationTest, Cheat) {
    uint8_t cheatType = CHEAT_LOSE;
    uint32_t clientId = 123;

    std::vector<uint8_t> bytes = {CHEAT_COMMAND, cheatType};

    ClientToServerCheat* cmd = ClientToServerCheat::from_bytes(bytes, clientId);

    ASSERT_NE(cmd, nullptr);
    EXPECT_EQ(cmd->cheat_type, cheatType);

    delete cmd;
}
