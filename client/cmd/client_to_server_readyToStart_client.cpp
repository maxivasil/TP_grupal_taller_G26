#include "client_to_server_readyToStart_client.h"

#include <utility>

ClientToServerReady_Client::ClientToServerReady_Client(const std::string& car,
                                                       const std::string& username):
        car(car), username(username) {}

std::vector<uint8_t> ClientToServerReady_Client::to_bytes() const {
    std::vector<uint8_t> data;
    BufferUtils::append_uint8(data, READY_TO_START_COMMAND);
    uint8_t len = car.size();
    BufferUtils::append_uint8(data, len);
    BufferUtils::append_bytes(data, car.data(), car.size());
    BufferUtils::append_bytes(data, username.data(), username.size());
    return data;
}
