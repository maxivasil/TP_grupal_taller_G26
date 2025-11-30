#include <cstring>
#include <vector>

#include <arpa/inet.h>
#include <gtest/gtest.h>

#include "../common/buffer_utils.h"
#include "../server/cmd/server_to_client_snapshot_server.h"


TEST(STCProtocolSerializationTest, Snapshot) {
    std::vector<CarSnapshot> cars = {
            {10, 100.5f, 200.75f, true, 95.0f, 30.2f, 45.0f, false, 2, true, true, "Player1"},
            {22, -10.0f, 0.5f, false, 80.0f, 70.0f, 180.0f, true, 5, false, false, "Player2"}};

    float elapsedTime = 12.34f;
    ServerToClientSnapshot_Server msg(cars, elapsedTime);
    std::vector<uint8_t> bytes = msg.to_bytes();

    ASSERT_GT(bytes.size(), 1);
    EXPECT_EQ(bytes[0], SNAPSHOT_COMMAND);

    size_t offset = 1;

    float readElapsedTime;
    BufferUtils::read_float(bytes, offset, readElapsedTime);
    EXPECT_FLOAT_EQ(readElapsedTime, elapsedTime);

    uint8_t count = bytes[offset++];
    EXPECT_EQ(count, cars.size());

    for (size_t i = 0; i < cars.size(); i++) {
        uint32_t id;
        BufferUtils::read_uint32(bytes, offset, id);
        EXPECT_EQ(id, cars[i].id);

        float f;
        auto checkFloat = [&](float expected) {
            BufferUtils::read_float(bytes, offset, f);
            EXPECT_FLOAT_EQ(f, expected);
        };

        checkFloat(cars[i].pos_x);
        checkFloat(cars[i].pos_y);

        EXPECT_EQ(bytes[offset++], cars[i].collision ? 1 : 0);
        checkFloat(cars[i].health);
        checkFloat(cars[i].speed);
        checkFloat(cars[i].angle);

        EXPECT_EQ(bytes[offset++], cars[i].onBridge ? 1 : 0);
        EXPECT_EQ(bytes[offset++], cars[i].car_type);
        EXPECT_EQ(bytes[offset++], cars[i].hasInfiniteHealth ? 1 : 0);
        EXPECT_EQ(bytes[offset++], cars[i].isNPC ? 1 : 0);

        // Validar nombre
        uint16_t nameLen;
        BufferUtils::read_uint16(bytes, offset, nameLen);
        EXPECT_EQ(nameLen, cars[i].playerName.length());
        if (nameLen > 0) {
            std::string readName((char*)&bytes[offset], nameLen);
            EXPECT_EQ(readName, cars[i].playerName);
            offset += nameLen;
        }
    }

    EXPECT_EQ(offset, bytes.size());
}
