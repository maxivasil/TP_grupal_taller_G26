#include "client_to_server_tour.h"

ClientToServerTour::ClientToServerTour(std::string& tourFile): tourFile(tourFile) {}

std::vector<uint8_t> ClientToServerTour::to_bytes() const {
    std::vector<uint8_t> data;
    BufferUtils::append_uint8(data, TOUR_COMMAND);
    BufferUtils::append_bytes(data, tourFile.data(), tourFile.size());
    return data;
}
