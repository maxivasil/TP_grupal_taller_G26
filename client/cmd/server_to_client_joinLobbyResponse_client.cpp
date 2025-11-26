#include "server_to_client_joinLobbyResponse_client.h"

#include <cstring>
#include <stdexcept>

#include <arpa/inet.h>

#include "../UI/initialwindow.h"
#include "../client_game.h"

ServerToClientJoinLobbyResponse_Client::ServerToClientJoinLobbyResponse_Client(uint8_t status,
                                                                               std::string lobbyId,
                                                                               uint8_t errorCode):
        status(status), lobbyId(lobbyId), errorCode(errorCode) {}


void ServerToClientJoinLobbyResponse_Client::execute(ClientContext& ctx) {
    InitialWindow* window = qobject_cast<InitialWindow*>(ctx.mainwindow);
    if (status == STATUS_OK) {
        window->changeScreen(lobbyId);
    } else if (status == STATUS_ERROR) {
        window->showError();
    }
}

ServerToClientJoinLobbyResponse_Client ServerToClientJoinLobbyResponse_Client::from_bytes(
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
        return ServerToClientJoinLobbyResponse_Client(status, "", errorCode);
    }
    std::string lobbyId(data.begin() + 2, data.begin() + 8);
    return ServerToClientJoinLobbyResponse_Client(status, lobbyId, -1);
}

uint8_t ServerToClientJoinLobbyResponse_Client::get_only_for_test_status() const { return status; }

const std::string& ServerToClientJoinLobbyResponse_Client::get_only_for_test_lobbyId() const {
    return lobbyId;
}

uint8_t ServerToClientJoinLobbyResponse_Client::get_only_for_test_errorCode() const {
    return errorCode;
}
