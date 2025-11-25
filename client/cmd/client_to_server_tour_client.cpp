#include "client_to_server_tour_client.h"

ClientToServerTour_Client::ClientToServerTour_Client(std::string& tourFile): tourFile(tourFile) {}

std::vector<uint8_t> ClientToServerTour_Client::to_bytes() const {
    std::vector<uint8_t> data;
    BufferUtils::append_uint8(data, TOUR_COMMAND);
    BufferUtils::append_bytes(data, tourFile.data(), tourFile.size());
    return data;
}
