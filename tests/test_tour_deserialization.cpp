#include <memory>

#include <gtest/gtest.h>

#include "../common/buffer_utils.h"

#include "testable_client_to_server.h"

TEST(CTSProtocolDeserializationTest, TourDeserialization) {
    std::string tour = "tour.yaml";
    uint32_t clientId = 123;

    std::vector<uint8_t> bytes;
    BufferUtils::append_uint8(bytes, TOUR_COMMAND);
    BufferUtils::append_bytes(bytes, tour.data(), tour.size());

    std::unique_ptr<ClientToServerTour> cmd(ClientToServerTour::from_bytes(bytes, clientId));

    ASSERT_NE(cmd, nullptr);
    EXPECT_EQ(cmd->tourFile, tour);
}
