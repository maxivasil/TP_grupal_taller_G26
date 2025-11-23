#include "server_to_client_assign_id.h"

#include <iostream>

#include <arpa/inet.h>

#include "../../common/buffer_utils.h"

ServerToClientAssignId::ServerToClientAssignId(uint32_t client_id): client_id(client_id) {}

std::vector<uint8_t> ServerToClientAssignId::to_bytes() const {
    std::vector<uint8_t> data;

    BufferUtils::append_uint8(data, ASSIGN_ID_COMMAND);

    BufferUtils::append_uint32(data, client_id);

    return data;
}

ServerToClientCmd_Server* ServerToClientAssignId::clone() const {
    return new ServerToClientAssignId(*this);
}
