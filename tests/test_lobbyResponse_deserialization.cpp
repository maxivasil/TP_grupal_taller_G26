#include <cstdint>
#include <cstring>
#include <vector>

#include <gtest/gtest.h>

#include "../common/buffer_utils.h"

#include "testable_server_to_client.h"

TEST(STCProtocolDeserializationTest, lobbyResponseStatusOK) {
    std::vector<uint8_t> bytes;
    BufferUtils::append_uint8(bytes, JOIN_RESPONSE_COMMAND);
    BufferUtils::append_uint8(bytes, STATUS_OK);

    std::string lobbyId = "XYZ789";
    BufferUtils::append_bytes(bytes, lobbyId.data(), lobbyId.size());

    auto msg = ServerToClientLobbyResponse::from_bytes(bytes);

    EXPECT_EQ(msg.status, STATUS_OK);
    EXPECT_EQ(msg.lobbyId, lobbyId);
    EXPECT_EQ(msg.errorCode, (uint8_t)-1);
}

TEST(STCProtocolDeserializationTest, lobbyResponseStatusError) {
    std::vector<uint8_t> bytes;
    BufferUtils::append_uint8(bytes, JOIN_RESPONSE_COMMAND);
    BufferUtils::append_uint8(bytes, STATUS_ERROR);
    BufferUtils::append_uint8(bytes, 4);

    auto msg = ServerToClientLobbyResponse::from_bytes(bytes);

    EXPECT_EQ(msg.status, STATUS_ERROR);
    EXPECT_EQ(msg.lobbyId, "");
    EXPECT_EQ(msg.errorCode, 4);
}
