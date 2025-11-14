#include "server_to_client_assign_id.h"

#include <iostream>

#include "../../common/buffer_utils.h"

ServerToClientAssignId::ServerToClientAssignId(uint8_t client_id): client_id(client_id) {}

std::vector<uint8_t> ServerToClientAssignId::to_bytes() const {
    std::vector<uint8_t> data;

    uint8_t header = ASSIGN_ID_COMMAND;
    data.push_back(header);

    BufferUtils::append_bytes(data, &client_id, sizeof(client_id));

    return data;
}

ServerToClientCmd_Server* ServerToClientAssignId::clone() const {
    return new ServerToClientAssignId(*this);
}
