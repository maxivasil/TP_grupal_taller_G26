#include <vector>

#include <gtest/gtest.h>

#include "../client/cmd/server_to_client_accumulatedResults_client.h"
#include "../common/buffer_utils.h"

TEST(STCProtocolDeserializationTest, AccumulatedResults) {
    std::vector<uint8_t> bytes;

    BufferUtils::append_uint8(bytes, ACCUMULATED_RESULTS_COMMAND);

    BufferUtils::append_uint16(bytes, 2);

    BufferUtils::append_uint32(bytes, 0);
    BufferUtils::append_uint16(bytes, 1);
    BufferUtils::append_float(bytes, 12.34f);

    BufferUtils::append_uint32(bytes, 1);
    BufferUtils::append_uint16(bytes, 2);
    BufferUtils::append_float(bytes, 15.99f);

    auto msg = ServerToClientAccumulatedResults_Client::from_bytes(bytes);

    std::vector<AccumulatedResultDTO> results = msg.get_only_for_test_results();

    ASSERT_EQ(results.size(), 2);

    EXPECT_EQ(results[0].playerId, 0);
    EXPECT_EQ(results[0].completedRaces, 1);
    EXPECT_FLOAT_EQ(results[0].totalTime, 12.34f);

    EXPECT_EQ(results[1].playerId, 1);
    EXPECT_EQ(results[1].completedRaces, 2);
    EXPECT_FLOAT_EQ(results[1].totalTime, 15.99f);
}
