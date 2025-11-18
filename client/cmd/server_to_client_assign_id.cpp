#include "server_to_client_assign_id.h"

#include <iostream>

#include "../client_game.h"

ServerToClientAssignId::ServerToClientAssignId(uint8_t client_id): client_id(client_id) {}

void ServerToClientAssignId::execute(ClientContext& ctx) {
    if (ctx.game) {
        ctx.game->setClientId(client_id);
        std::cout << "[CLIENT] Assigned ID: " << int(client_id) << std::endl;
    }
}

ServerToClientAssignId ServerToClientAssignId::from_bytes(const std::vector<uint8_t>& data) {
    return ServerToClientAssignId(data[1]);
}
