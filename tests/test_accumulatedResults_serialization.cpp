#include <cstdint>
#include <cstring>
#include <vector>

#include <gtest/gtest.h>

#include "../server/cmd/server_to_client_accumulatedResults.h"

TEST(ProtocolSerializationTest, AccumulatedResultsToBytes) {
    std::vector<AccumulatedResultDTO> results = {{0, 1, 12.34f}, {1, 2, 15.99f}};

    ServerToClientAccumulatedResults msg(results);
    std::vector<uint8_t> bytes = msg.to_bytes();

    ASSERT_GT(bytes.size(), 1);

    EXPECT_EQ(bytes[0], ACCUMULATED_RESULTS_COMMAND);

    size_t offset = 1;

    EXPECT_EQ(bytes[offset], results[0].playerId);
    offset += 1;

    uint16_t completedRaces0 = static_cast<uint16_t>(bytes[offset] | (bytes[offset + 1] << 8));
    EXPECT_EQ(completedRaces0, results[0].completedRaces);
    offset += 2;

    float totalTime0;
    std::memcpy(&totalTime0, &bytes[offset], sizeof(float));
    EXPECT_FLOAT_EQ(totalTime0, results[0].totalTime);
    offset += sizeof(float);

    EXPECT_EQ(bytes[offset], results[1].playerId);
    offset += 1;

    uint16_t completedRaces1 = static_cast<uint16_t>(bytes[offset] | (bytes[offset + 1] << 8));
    EXPECT_EQ(completedRaces1, results[1].completedRaces);
    offset += 2;

    float totalTime1;
    std::memcpy(&totalTime1, &bytes[offset], sizeof(float));
    EXPECT_FLOAT_EQ(totalTime1, results[1].totalTime);
    offset += sizeof(float);

    EXPECT_EQ(offset, bytes.size());
}
