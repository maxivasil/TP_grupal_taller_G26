#include "client_to_server_readyToStart.h"

ClientToServerReady::ClientToServerReady(std::string car): car(car) {}

std::vector<uint8_t> ClientToServerReady::to_bytes() const {
    std::vector<uint8_t> data;
    BufferUtils::append_uint8(data, READY_TO_START_COMMAND);
    BufferUtils::append_bytes(data, car.data(), car.size());
    return data;
}
