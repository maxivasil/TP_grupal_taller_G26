#include "server_to_client_joinLobbyResponse.h"

#include <stdexcept>

/**
 * <HEADER>;<STATUS>;<ERROR_CODE/LOBBY_ID>
 *  If status is STATUS_OK -> LOBBY_ID
 *  If status is STATUS_ERROR -> ERROR_CODE
 */

ServerToClientJoinResponse::ServerToClientJoinResponse(uint8_t status, const std::string& lobbyId):
        status(status), lobbyId(lobbyId), errorCode(0) {
    if (status != STATUS_OK) {
        throw std::invalid_argument("Error: Este constructor solo es para STATUS_OK.");
    }
    if (lobbyId.size() != 6) {
        throw std::invalid_argument("Error: El lobbyId debe tener exactamente 6 caracteres.");
    }
}

ServerToClientJoinResponse::ServerToClientJoinResponse(uint8_t status, uint8_t errorCode):
        status(status), lobbyId(""), errorCode(errorCode) {
    if (status != STATUS_ERROR) {
        throw std::invalid_argument("Error: Este constructor solo es para STATUS_ERROR.");
    }
}

std::vector<uint8_t> ServerToClientJoinResponse::to_bytes() const {
    std::vector<uint8_t> data;

    BufferUtils::append_uint8(data, JOIN_RESPONSE_COMMAND);
    BufferUtils::append_uint8(data, status);
    if (status == STATUS_OK) {
        BufferUtils::append_bytes(data, lobbyId.data(), lobbyId.size());
    } else {
        BufferUtils::append_uint8(data, errorCode);
    }
    return data;
}

ServerToClientJoinResponse* ServerToClientJoinResponse::clone() const {
    return new ServerToClientJoinResponse(*this);
}
