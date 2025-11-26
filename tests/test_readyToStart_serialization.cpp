#include <vector>

#include <gtest/gtest.h>

#include "../client/cmd/client_to_server_readyToStart_client.h"


TEST(CTSProtocolSerializationTest, ReadyToStart) {
    std::string car = "Ferrari";
    std::string username = "TestReadyToStartSerialization";
    ClientToServerReady_Client cmd(car, username);

    std::vector<uint8_t> bytes = cmd.to_bytes();

    ASSERT_GE(bytes.size(), 2);

    EXPECT_EQ(bytes[0], READY_TO_START_COMMAND);

    uint8_t expected_len = car.size();
    EXPECT_EQ(bytes[1], expected_len);

    std::string extracted_car(bytes.begin() + 2, bytes.begin() + 2 + expected_len);
    EXPECT_EQ(extracted_car, car);

    std::string extracted_username(bytes.begin() + 2 + expected_len, bytes.end());
    EXPECT_EQ(extracted_username, username);

    EXPECT_EQ(bytes.size(), 1 + 1 + car.size() + username.size());
}
