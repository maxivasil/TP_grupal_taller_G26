#include <vector>

#include <gtest/gtest.h>

#include "../common/buffer_utils.h"

#include "testable_client_to_server.h"

TEST(CTSProtocolDeserializationTest, Lobby_Create) {
    std::vector<uint8_t> bytes;
    BufferUtils::append_uint8(bytes, JOIN_COMMAND);
    BufferUtils::append_uint8(bytes, TYPE_CREATE);

    std::string lobbyId = "ABC123";
    BufferUtils::append_bytes(bytes, lobbyId.data(), lobbyId.size());

    uint32_t clientId = 55;
    ClientToServerJoinLobby* cmd = ClientToServerJoinLobby::from_bytes(bytes, clientId);

    ASSERT_NE(cmd, nullptr);
    EXPECT_EQ(cmd->lobbyId, lobbyId);
    EXPECT_EQ(cmd->type, TYPE_CREATE);

    delete cmd;
}

TEST(CTSProtocolDeserializationTest, Lobby_Join) {
    std::vector<uint8_t> bytes;
    BufferUtils::append_uint8(bytes, JOIN_COMMAND);
    BufferUtils::append_uint8(bytes, TYPE_JOIN);

    std::string lobbyId = "XYZ789";
    BufferUtils::append_bytes(bytes, lobbyId.data(), lobbyId.size());

    uint32_t clientId = 99;
    ClientToServerJoinLobby* cmd = ClientToServerJoinLobby::from_bytes(bytes, clientId);

    ASSERT_NE(cmd, nullptr);
    EXPECT_EQ(cmd->lobbyId, lobbyId);
    EXPECT_EQ(cmd->type, TYPE_JOIN);

    delete cmd;
}
