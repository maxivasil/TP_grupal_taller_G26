#include <vector>

#include <gtest/gtest.h>

#include "../client/cmd/client_to_server_tour.h"

TEST(CTSProtocolSerializationTest, Tour) {
    std::string tour = "tour.yaml";
    ClientToServerTour cmd(tour);

    std::vector<uint8_t> bytes = cmd.to_bytes();

    ASSERT_EQ(bytes[0], TOUR_COMMAND);

    std::string extracted(bytes.begin() + 1, bytes.end());
    EXPECT_EQ(extracted, tour);

    EXPECT_EQ(bytes.size(), 1 + tour.size());
}
