#include "client_to_server_move_client.h"

ClientToServerMove_Client::ClientToServerMove_Client(uint8_t direction): direction(direction) {}

std::vector<uint8_t> ClientToServerMove_Client::to_bytes() const {
    std::vector<uint8_t> data;
    BufferUtils::append_uint8(data, MOVE_COMMAND);
    BufferUtils::append_uint8(data, direction);
    return data;
}
