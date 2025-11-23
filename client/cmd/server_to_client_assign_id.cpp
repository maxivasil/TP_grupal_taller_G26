#include "server_to_client_assign_id.h"

#include <cstring>
#include <iostream>
#include <stdexcept>

#include "../client_game.h"

ServerToClientAssignId::ServerToClientAssignId(uint32_t client_id): client_id(client_id) {}

void ServerToClientAssignId::execute(ClientContext& ctx) {
    if (ctx.game) {
        ctx.game->setClientId(client_id);
    }
}

ServerToClientAssignId ServerToClientAssignId::from_bytes(const std::vector<uint8_t>& data) {
    if (data[0] != ASSIGN_ID_COMMAND || data.size() < 5) {
        throw std::runtime_error("Invalid data for ServerToClientAssignId");
    }

    uint32_t client_id;
    size_t offset = 1;
    BufferUtils::read_uint32(data, offset, client_id);
    return ServerToClientAssignId(client_id);
}
