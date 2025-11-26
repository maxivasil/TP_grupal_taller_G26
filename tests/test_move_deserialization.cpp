#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "../server/cmd/client_to_server_move_server.h"

TEST(CTSProtocolDeserializationTest, Move) {
    uint8_t direction = MOVE_RIGHT;
    uint32_t clientId = 42;

    std::vector<uint8_t> bytes = {MOVE_COMMAND, direction};

    std::unique_ptr<ClientToServerMove_Server> cmd(
            ClientToServerMove_Server::from_bytes(bytes, clientId));

    ASSERT_NE(cmd, nullptr);
    EXPECT_EQ(cmd->get_only_for_test_direction(), direction);
}
