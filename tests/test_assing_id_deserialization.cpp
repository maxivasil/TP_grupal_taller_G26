#include <vector>

#include <gtest/gtest.h>

#include "../common/buffer_utils.h"

#include "testable_server_to_client.h"

TEST(STCProtocolDeserializationTest, AssignId) {
    std::vector<uint8_t> bytes;

    BufferUtils::append_uint8(bytes, ASSIGN_ID_COMMAND);

    uint32_t client_id = 123456;
    BufferUtils::append_uint32(bytes, client_id);

    auto msg = ServerToClientAssignId::from_bytes(bytes);

    EXPECT_EQ(msg.client_id, client_id);
}
