#include "client_to_server_cheat_client.h"

#include "../../common/buffer_utils.h"

ClientToServerCheat_Client::ClientToServerCheat_Client(uint8_t cheat_type):
        cheat_type(cheat_type) {}

std::vector<uint8_t> ClientToServerCheat_Client::to_bytes() const {
    std::vector<uint8_t> result;
    BufferUtils::append_uint8(result, CHEAT_COMMAND);
    BufferUtils::append_uint8(result, cheat_type);
    return result;
}
