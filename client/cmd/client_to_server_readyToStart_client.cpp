#include "client_to_server_readyToStart_client.h"

#include <utility>

ClientToServerReady_Client::ClientToServerReady_Client(std::string car): car(std::move(car)) {}

std::vector<uint8_t> ClientToServerReady_Client::to_bytes() const {
    std::vector<uint8_t> data;
    BufferUtils::append_uint8(data, READY_TO_START_COMMAND);
    BufferUtils::append_bytes(data, car.data(), car.size());
    return data;
}
