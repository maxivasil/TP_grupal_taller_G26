#include "server_to_client_lobbyResponse.h"

#include <cstring>
#include <stdexcept>

#include <arpa/inet.h>

#include "../client_game.h"
#include "../UI/initialwindow.h"

ServerToClientLobbyResponse::ServerToClientLobbyResponse(uint8_t status, std::string lobbyId,
                                                         uint8_t errorCode):
        status(status), lobbyId(lobbyId), errorCode(errorCode) {}


void ServerToClientLobbyResponse::execute(ClientContext& ctx) {
    InitialWindow* window = qobject_cast<InitialWindow*>(ctx.mainwindow);
    if (status == STATUS_OK) {
        window->changeScreen(lobbyId);
    }
    else if (status == STATUS_ERROR) {
        window->showError();
    }
}

ServerToClientLobbyResponse ServerToClientLobbyResponse::from_bytes(
        const std::vector<uint8_t>& data) {
    if (data.empty()) {
        throw std::runtime_error("Join Response data is empty");
    }

    size_t offset = 0;

    uint8_t header = data[offset++];
    if (header != JOIN_RESPONSE_COMMAND)
        throw std::runtime_error("Invalid header for Join Response");

    uint8_t status = data[offset++];
    if (status == STATUS_ERROR) {
        uint8_t errorCode = data[offset++];
        return ServerToClientLobbyResponse(status, "", errorCode);
    }
    std::string lobbyId(data.begin() + 2, data.begin() + 8);
    return ServerToClientLobbyResponse(status, lobbyId, -1);
}
