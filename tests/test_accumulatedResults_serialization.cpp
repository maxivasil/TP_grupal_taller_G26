#include <cstdint>
#include <cstring>
#include <vector>

#include <arpa/inet.h>
#include <gtest/gtest.h>

#include "../server/cmd/server_to_client_accumulatedResults.h"

TEST(ProtocolSerializationTest, AccumulatedResultsToBytes) {
    std::vector<AccumulatedResultDTO> results = {{0, 1, 12.34f}, {1, 2, 15.99f}};

    ServerToClientAccumulatedResults msg(results);
    std::vector<uint8_t> bytes = msg.to_bytes();

    ASSERT_GT(bytes.size(), 1);

    EXPECT_EQ(bytes[0], ACCUMULATED_RESULTS_COMMAND);

    size_t offset = 1;

    uint16_t count;
    std::memcpy(&count, &bytes[offset], sizeof(uint16_t));
    count = ntohs(count);
    EXPECT_EQ(count, results.size());
    offset += sizeof(uint16_t);

    uint32_t pid0;
    std::memcpy(&pid0, &bytes[offset], sizeof(uint32_t));
    pid0 = ntohl(pid0);
    EXPECT_EQ(pid0, results[0].playerId);
    offset += sizeof(uint32_t);

    uint16_t completedRaces0;
    std::memcpy(&completedRaces0, &bytes[offset], sizeof(uint16_t));
    completedRaces0 = ntohs(completedRaces0);
    EXPECT_EQ(completedRaces0, results[0].completedRaces);
    offset += sizeof(uint16_t);

    uint32_t timeBits0;
    std::memcpy(&timeBits0, &bytes[offset], sizeof(uint32_t));
    timeBits0 = ntohl(timeBits0);

    float totalTime0;
    std::memcpy(&totalTime0, &timeBits0, sizeof(float));
    EXPECT_FLOAT_EQ(totalTime0, results[0].totalTime);
    offset += sizeof(uint32_t);

    uint32_t pid1;
    std::memcpy(&pid1, &bytes[offset], sizeof(uint32_t));
    pid1 = ntohl(pid1);
    EXPECT_EQ(pid1, results[1].playerId);
    offset += sizeof(uint32_t);

    uint16_t completedRaces1;
    std::memcpy(&completedRaces1, &bytes[offset], sizeof(uint16_t));
    completedRaces1 = ntohs(completedRaces1);
    EXPECT_EQ(completedRaces1, results[1].completedRaces);
    offset += sizeof(uint16_t);

    uint32_t timeBits1;
    std::memcpy(&timeBits1, &bytes[offset], sizeof(uint32_t));
    timeBits1 = ntohl(timeBits1);

    float totalTime1;
    std::memcpy(&totalTime1, &timeBits1, sizeof(float));
    EXPECT_FLOAT_EQ(totalTime1, results[1].totalTime);
    offset += sizeof(uint32_t);

    EXPECT_EQ(offset, bytes.size());
}
