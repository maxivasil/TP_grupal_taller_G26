#include "client_to_server_cheat.h"

#include "../../common/buffer_utils.h"

ClientToServerCheat::ClientToServerCheat(uint8_t cheat_type): cheat_type(cheat_type) {}

std::vector<uint8_t> ClientToServerCheat::to_bytes() const {
    std::vector<uint8_t> result;
    BufferUtils::append_uint8(result, CHEAT_COMMAND);
    BufferUtils::append_uint8(result, cheat_type);
    return result;
}
