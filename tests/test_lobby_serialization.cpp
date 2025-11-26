#include <vector>

#include <gtest/gtest.h>

#include "../client/cmd/client_to_server_joinLobby_client.h"

TEST(CTSProtocolSerializationTest, Lobby_Create) {
    std::string lobbyId = "ABC123";
    ClientToServerJoinLobby_Client cmd(lobbyId, true);  // TYPE_CREATE

    std::vector<uint8_t> bytes = cmd.to_bytes();

    ASSERT_EQ(bytes[0], JOIN_COMMAND);
    ASSERT_EQ(bytes[1], TYPE_CREATE);

    std::string extracted(bytes.begin() + 2, bytes.begin() + 8);
    EXPECT_EQ(extracted, lobbyId);

    EXPECT_EQ(bytes.size(), 1 + 1 + 6);
}

TEST(CTSProtocolSerializationTest, Lobby_Join) {
    std::string lobbyId = "XYZ789";
    ClientToServerJoinLobby_Client cmd(lobbyId, false);  // TYPE_JOIN

    std::vector<uint8_t> bytes = cmd.to_bytes();

    ASSERT_EQ(bytes[0], JOIN_COMMAND);
    ASSERT_EQ(bytes[1], TYPE_JOIN);

    std::string extracted(bytes.begin() + 2, bytes.begin() + 8);
    EXPECT_EQ(extracted, lobbyId);

    EXPECT_EQ(bytes.size(), 1 + 1 + 6);
}
