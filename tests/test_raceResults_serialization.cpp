#include <cstring>
#include <vector>

#include <arpa/inet.h>
#include <gtest/gtest.h>

#include "../common/buffer_utils.h"
#include "../server/cmd/server_to_client_raceResults.h"

TEST(STCProtocolSerializationTest, RaceResults) {
    std::vector<PlayerResult> players = {{1, "Pedro", 65.123f, 1}, {2, "Lucia", 70.456f, 2}};

    ServerToClientRaceResults msg(players, true);
    std::vector<uint8_t> bytes = msg.to_bytes();

    ASSERT_GT(bytes.size(), 1);
    EXPECT_EQ(bytes[0], SERVER_TO_CLIENT_RACE_RESULTS);

    size_t offset = 1;

    uint8_t finishedFlag = bytes[offset++];
    EXPECT_EQ(finishedFlag, 1);

    uint8_t playerCount = bytes[offset++];
    EXPECT_EQ(playerCount, players.size());

    for (size_t i = 0; i < players.size(); i++) {
        uint32_t pid;
        BufferUtils::read_uint32(bytes, offset, pid);
        EXPECT_EQ(pid, players[i].playerId);

        uint16_t nameLen;
        BufferUtils::read_uint16(bytes, offset, nameLen);
        EXPECT_EQ(nameLen, players[i].playerName.size());

        std::string name(reinterpret_cast<char*>(&bytes[offset]), nameLen);
        EXPECT_EQ(name, players[i].playerName);
        offset += nameLen;

        float finishTime;
        BufferUtils::read_float(bytes, offset, finishTime);
        EXPECT_FLOAT_EQ(finishTime, players[i].finishTime);

        uint8_t position;
        BufferUtils::read_uint8(bytes, offset, position);
        EXPECT_EQ(position, players[i].position);
    }

    EXPECT_EQ(offset, bytes.size());
}
