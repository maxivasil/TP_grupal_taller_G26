#include "client_to_server_readyToStart.h"

ClientToServerReady::ClientToServerReady(const std::string& car, const std::string& username):
        car(car), username(username) {}

std::vector<uint8_t> ClientToServerReady::to_bytes() const {
    std::vector<uint8_t> data;
    data.push_back(READY_TO_START_COMMAND);
    uint8_t len = car.size();
    BufferUtils::append_bytes(data, &len, sizeof(len));
    BufferUtils::append_bytes(data, car.data(), car.size());
    BufferUtils::append_bytes(data, username.data(), username.size());
    return data;
}
