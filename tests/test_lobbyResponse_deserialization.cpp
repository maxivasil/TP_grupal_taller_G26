#include <cstdint>
#include <cstring>
#include <vector>

#include <gtest/gtest.h>

#include "../client/cmd/server_to_client_joinLobbyResponse_client.h"
#include "../common/buffer_utils.h"

TEST(STCProtocolDeserializationTest, lobbyResponseStatusOK) {
    std::vector<uint8_t> bytes;
    BufferUtils::append_uint8(bytes, JOIN_RESPONSE_COMMAND);
    BufferUtils::append_uint8(bytes, STATUS_OK);

    std::string lobbyId = "XYZ789";
    BufferUtils::append_bytes(bytes, lobbyId.data(), lobbyId.size());

    auto msg = ServerToClientJoinLobbyResponse_Client::from_bytes(bytes);

    EXPECT_EQ(msg.get_only_for_test_status(), STATUS_OK);
    EXPECT_EQ(msg.get_only_for_test_lobbyId(), lobbyId);
    EXPECT_EQ(msg.get_only_for_test_errorCode(), (uint8_t)-1);
}

TEST(STCProtocolDeserializationTest, lobbyResponseStatusError) {
    std::vector<uint8_t> bytes;
    BufferUtils::append_uint8(bytes, JOIN_RESPONSE_COMMAND);
    BufferUtils::append_uint8(bytes, STATUS_ERROR);
    BufferUtils::append_uint8(bytes, 4);

    auto msg = ServerToClientJoinLobbyResponse_Client::from_bytes(bytes);

    EXPECT_EQ(msg.get_only_for_test_status(), STATUS_ERROR);
    EXPECT_EQ(msg.get_only_for_test_lobbyId(), "");
    EXPECT_EQ(msg.get_only_for_test_errorCode(), 4);
}
