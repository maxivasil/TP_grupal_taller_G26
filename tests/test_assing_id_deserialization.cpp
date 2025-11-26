#include <vector>

#include <gtest/gtest.h>

#include "../client/cmd/server_to_client_assign_id_client.h"
#include "../common/buffer_utils.h"

TEST(STCProtocolDeserializationTest, AssignId) {
    std::vector<uint8_t> bytes;

    BufferUtils::append_uint8(bytes, ASSIGN_ID_COMMAND);

    uint32_t client_id = 123456;
    BufferUtils::append_uint32(bytes, client_id);

    auto msg = ServerToClientAssignId_Client::from_bytes(bytes);

    uint32_t deserialized_client_id = msg.get_only_for_test_client_id();

    EXPECT_EQ(deserialized_client_id, client_id);
}
