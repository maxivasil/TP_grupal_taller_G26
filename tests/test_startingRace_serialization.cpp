#include <cstring>
#include <vector>

#include <arpa/inet.h>
#include <gtest/gtest.h>

#include "../server/cmd/server_to_client_startingRace_server.h"

TEST(STCProtocolSerializationTest, StartingRace) {
    uint8_t cityId = 3;
    std::string trackFile = "liberty_city.map";

    ServerToClientStartingRace_Server msg(cityId, trackFile, false);
    auto bytes = msg.to_bytes();

    ASSERT_GT(bytes.size(), 2);
    EXPECT_EQ(bytes[0], STARTING_RACE_COMMAND);
    EXPECT_EQ(bytes[1], cityId);
    EXPECT_EQ(bytes[2], 0);

    std::string file(reinterpret_cast<const char*>(&bytes[3]), bytes.size() - 3);
    EXPECT_EQ(file, trackFile);
}
