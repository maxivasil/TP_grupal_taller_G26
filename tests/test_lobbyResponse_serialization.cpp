#include <vector>

#include <gtest/gtest.h>

#include "../server/cmd/server_to_client_joinLobbyResponse.h"

TEST(STCProtocolSerializationTest, lobbyResponseStatusOK) {
    std::string lobbyId = "ABC123";
    ServerToClientJoinResponse cmd(STATUS_OK, lobbyId);

    std::vector<uint8_t> bytes = cmd.to_bytes();

    ASSERT_EQ(bytes[0], JOIN_RESPONSE_COMMAND);
    ASSERT_EQ(bytes[1], STATUS_OK);

    std::string extracted(bytes.begin() + 2, bytes.begin() + 8);
    EXPECT_EQ(extracted, lobbyId);

    EXPECT_EQ(bytes.size(), 1 + 1 + 6);
}

TEST(STCProtocolSerializationTest, lobbyResponseStatusError) {
    uint8_t errorCode = 7;
    ServerToClientJoinResponse cmd(STATUS_ERROR, errorCode);

    std::vector<uint8_t> bytes = cmd.to_bytes();

    ASSERT_EQ(bytes[0], JOIN_RESPONSE_COMMAND);
    ASSERT_EQ(bytes[1], STATUS_ERROR);
    ASSERT_EQ(bytes[2], errorCode);

    EXPECT_EQ(bytes.size(), 1 + 1 + 1);
}
