#include <cstring>
#include <vector>

#include <arpa/inet.h>
#include <gtest/gtest.h>

#include "../common/buffer_utils.h"

#include "testable_server_to_client.h"

TEST(STCProtocolDeserializationTest, FromBytes_CorrectData) {
    uint8_t cityId = 4;
    std::string trackFile = "vice_city.map";

    std::vector<uint8_t> bytes;
    BufferUtils::append_uint8(bytes, STARTING_RACE_COMMAND);
    BufferUtils::append_uint8(bytes, cityId);

    BufferUtils::append_bytes(bytes, trackFile.data(), trackFile.size());

    auto cmd = ServerToClientStartingRace::from_bytes(bytes);

    EXPECT_EQ(cmd.cityId, cityId);
    EXPECT_EQ(cmd.trackFile, trackFile);
}
