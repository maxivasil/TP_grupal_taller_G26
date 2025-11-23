#include <cstdint>
#include <cstring>
#include <vector>

#include <arpa/inet.h>
#include <gtest/gtest.h>

#include "../common/buffer_utils.h"
#include "../server/cmd/server_to_client_accumulatedResults.h"

TEST(STCProtocolSerializationTest, AccumulatedResults) {
    std::vector<AccumulatedResultDTO> results = {{0, 1, 12.34f}, {1, 2, 15.99f}};

    ServerToClientAccumulatedResults msg(results);
    std::vector<uint8_t> bytes = msg.to_bytes();

    ASSERT_GT(bytes.size(), 1);

    EXPECT_EQ(bytes[0], ACCUMULATED_RESULTS_COMMAND);

    size_t offset = 1;

    uint16_t count;
    BufferUtils::read_uint16(bytes, offset, count);
    EXPECT_EQ(count, results.size());

    uint32_t pid0;
    BufferUtils::read_uint32(bytes, offset, pid0);
    EXPECT_EQ(pid0, results[0].playerId);

    uint16_t completedRaces0;
    BufferUtils::read_uint16(bytes, offset, completedRaces0);
    EXPECT_EQ(completedRaces0, results[0].completedRaces);

    float totalTime0;
    BufferUtils::read_float(bytes, offset, totalTime0);
    EXPECT_FLOAT_EQ(totalTime0, results[0].totalTime);

    uint32_t pid1;
    BufferUtils::read_uint32(bytes, offset, pid1);
    EXPECT_EQ(pid1, results[1].playerId);

    uint16_t completedRaces1;
    BufferUtils::read_uint16(bytes, offset, completedRaces1);
    EXPECT_EQ(completedRaces1, results[1].completedRaces);

    float totalTime1;
    BufferUtils::read_float(bytes, offset, totalTime1);
    EXPECT_FLOAT_EQ(totalTime1, results[1].totalTime);

    EXPECT_EQ(offset, bytes.size());
}
