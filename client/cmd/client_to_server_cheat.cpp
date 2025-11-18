#include "client_to_server_cheat.h"

ClientToServerCheat::ClientToServerCheat(uint8_t cheat_type): cheat_type(cheat_type) {}

std::vector<uint8_t> ClientToServerCheat::to_bytes() const {
    std::vector<uint8_t> result;
    result.push_back(CHEAT_COMMAND);
    result.push_back(cheat_type);
    return result;
}
