#include "server_to_client_assign_id_client.h"

#include <cstring>
#include <iostream>
#include <stdexcept>

#include "../client_game.h"

ServerToClientAssignId_Client::ServerToClientAssignId_Client(uint32_t client_id):
        client_id(client_id) {}

void ServerToClientAssignId_Client::execute(ClientContext& ctx) {
    if (ctx.game) {
        ctx.game->setClientId(client_id);
    }
}

ServerToClientAssignId_Client ServerToClientAssignId_Client::from_bytes(
        const std::vector<uint8_t>& data) {
    if (data[0] != ASSIGN_ID_COMMAND || data.size() < 5) {
        throw std::runtime_error("Invalid data for ServerToClientAssignId_Client");
    }

    uint32_t client_id;
    size_t offset = 1;
    BufferUtils::read_uint32(data, offset, client_id);
    return ServerToClientAssignId_Client(client_id);
}

uint32_t ServerToClientAssignId_Client::get_only_for_test_client_id() const { return client_id; }
