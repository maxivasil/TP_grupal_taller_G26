#include <cstdint>
#include <cstring>
#include <vector>

#include <arpa/inet.h>
#include <gtest/gtest.h>

#include "../common/buffer_utils.h"
#include "../server/cmd/server_to_client_assign_id_server.h"

TEST(STCProtocolSerializationTest, AssignId) {
    uint32_t client_id = 42;

    ServerToClientAssignId_Server msg(client_id);
    std::vector<uint8_t> bytes = msg.to_bytes();

    ASSERT_EQ(bytes.size(), 5);

    EXPECT_EQ(bytes[0], ASSIGN_ID_COMMAND);

    uint32_t client_id_read;
    size_t offset = 1;
    BufferUtils::read_uint32(bytes, offset, client_id_read);
    EXPECT_EQ(client_id_read, client_id);
}
