#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "../server/cmd/client_to_server_cheat_server.h"

TEST(CTSProtocolDeserializationTest, Cheat) {
    uint8_t cheatType = CHEAT_LOSE;
    uint32_t clientId = 123;

    std::vector<uint8_t> bytes = {CHEAT_COMMAND, cheatType};

    std::unique_ptr<ClientToServerCheat_Server> cmd(
            ClientToServerCheat_Server::from_bytes(bytes, clientId));

    uint8_t deserialized_cheat_type = cmd->get_only_for_test_cheat_type();

    ASSERT_NE(cmd, nullptr);
    EXPECT_EQ(deserialized_cheat_type, cheatType);
}
