#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "../common/buffer_utils.h"

#include "testable_client_to_server.h"

TEST(CTSProtocolDeserializationTest, ReadyToStart) {
    std::string car = "Mustang";
    uint32_t clientId = 101;

    std::vector<uint8_t> bytes;
    BufferUtils::append_uint8(bytes, READY_TO_START_COMMAND);
    BufferUtils::append_bytes(bytes, car.data(), car.size());

    std::unique_ptr<ClientToServerReady> cmd(ClientToServerReady::from_bytes(bytes, clientId));

    ASSERT_NE(cmd, nullptr);
    EXPECT_EQ(cmd->car, car);
}
