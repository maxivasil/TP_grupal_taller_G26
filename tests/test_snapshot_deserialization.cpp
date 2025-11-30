#include <cstring>
#include <vector>

#include <arpa/inet.h>
#include <gtest/gtest.h>

#include "../client/cmd/server_to_client_snapshot_client.h"
#include "../common/buffer_utils.h"

TEST(STCProtocolDeserializationTest, Snapshot) {
    std::vector<uint8_t> bytes;
    BufferUtils::append_uint8(bytes, SNAPSHOT_COMMAND);
    float elapsedTime = 12.34f;
    BufferUtils::append_float(bytes, elapsedTime);
    uint8_t carCount = 2;
    BufferUtils::append_uint8(bytes, carCount);

    auto appendCar = [&](uint32_t id, float x, float y, bool coll, float health, float speed,
                         float angle, bool bridge, uint8_t type, bool inf, bool npc, const std::string& name) {
        BufferUtils::append_uint32(bytes, id);

        BufferUtils::append_float(bytes, x);
        BufferUtils::append_float(bytes, y);

        uint8_t collision = coll ? 1 : 0;
        BufferUtils::append_bytes(bytes, &collision, sizeof(collision));

        BufferUtils::append_float(bytes, health);
        BufferUtils::append_float(bytes, speed);
        BufferUtils::append_float(bytes, angle);

        uint8_t bridgeVal = bridge ? 1 : 0;
        BufferUtils::append_bytes(bytes, &bridgeVal, sizeof(bridgeVal));
        BufferUtils::append_bytes(bytes, &type, sizeof(type));
        uint8_t infVal = inf ? 1 : 0;
        BufferUtils::append_bytes(bytes, &infVal, sizeof(infVal));
        uint8_t npcVal = npc ? 1 : 0;
        BufferUtils::append_bytes(bytes, &npcVal, sizeof(npcVal));

        // Agregar nombre
        uint16_t nameLen = static_cast<uint16_t>(name.length());
        BufferUtils::append_uint16(bytes, nameLen);
        if (nameLen > 0) {
            BufferUtils::append_bytes(bytes, (uint8_t*)name.c_str(), nameLen);
        }
    };

    appendCar(10, 100.5f, 200.75f, true, 95.0f, 30.2f, 45.0f, false, 2, true, true, "Player1");
    appendCar(22, -10.0f, 0.5f, false, 80.0f, 70.0f, 180.0f, true, 5, false, false, "Player2");
    auto cmd = ServerToClientSnapshot_Client::from_bytes(bytes);
    const auto& cars_snapshot = cmd.get_only_for_test_cars_snapshot();
    ASSERT_EQ(cars_snapshot.size(), 2);

    const auto& c0 = cars_snapshot[0];
    EXPECT_EQ(c0.id, 10);
    EXPECT_FLOAT_EQ(c0.pos_x, 100.5f);
    EXPECT_FLOAT_EQ(c0.pos_y, 200.75f);
    EXPECT_TRUE(c0.collision);
    EXPECT_FLOAT_EQ(c0.health, 95.0f);
    EXPECT_FLOAT_EQ(c0.speed, 30.2f);
    EXPECT_FLOAT_EQ(c0.angle, 45.0f);
    EXPECT_FALSE(c0.onBridge);
    EXPECT_EQ(c0.car_type, 2);
    EXPECT_TRUE(c0.hasInfiniteHealth);
    EXPECT_TRUE(c0.isNPC);
    EXPECT_EQ(c0.playerName, "Player1");

    const auto& c1 = cars_snapshot[1];
    EXPECT_EQ(c1.id, 22);
    EXPECT_FLOAT_EQ(c1.pos_x, -10.0f);
    EXPECT_FLOAT_EQ(c1.pos_y, 0.5f);
    EXPECT_FALSE(c1.collision);
    EXPECT_FLOAT_EQ(c1.health, 80.0f);
    EXPECT_FLOAT_EQ(c1.speed, 70.0f);
    EXPECT_FLOAT_EQ(c1.angle, 180.0f);
    EXPECT_TRUE(c1.onBridge);
    EXPECT_EQ(c1.car_type, 5);
    EXPECT_FALSE(c1.hasInfiniteHealth);
    EXPECT_FALSE(c1.isNPC);
    EXPECT_EQ(c1.playerName, "Player2");
}
