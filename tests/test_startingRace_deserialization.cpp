#include <cstring>
#include <vector>

#include <arpa/inet.h>
#include <gtest/gtest.h>

#include "../client/cmd/server_to_client_startingRace_client.h"
#include "../common/buffer_utils.h"

TEST(STCProtocolDeserializationTest, FromBytes_CorrectData) {
    uint8_t cityId = 4;
    std::string trackFile = "vice_city.map";

    std::vector<uint8_t> bytes;
    BufferUtils::append_uint8(bytes, STARTING_RACE_COMMAND);
    BufferUtils::append_uint8(bytes, cityId);

    BufferUtils::append_bytes(bytes, trackFile.data(), trackFile.size());

    auto cmd = ServerToClientStartingRace_Client::from_bytes(bytes);

    EXPECT_EQ(cmd.get_only_for_test_cityId(), cityId);
    EXPECT_EQ(cmd.get_only_for_test_trackFile(), trackFile);
}
