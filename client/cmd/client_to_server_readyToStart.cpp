#include "client_to_server_readyToStart.h"

ClientToServerReady::ClientToServerReady() {}

std::vector<uint8_t> ClientToServerReady::to_bytes() const {
    std::vector<uint8_t> data;
    data.push_back(READY_TO_START_COMMAND);
    return data;
}
