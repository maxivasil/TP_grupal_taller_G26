#include "client_to_server_lobby.h"

#include <string>

ClientToServerLobby::ClientToServerLobby(std::string lobbyId, bool isCreate):
        lobbyId(lobbyId), isCreate(isCreate) {}

std::vector<uint8_t> ClientToServerLobby::to_bytes() const {
    std::vector<uint8_t> data;
    data.push_back(JOIN_COMMAND);
    data.push_back(isCreate ? TYPE_CREATE : TYPE_JOIN);
    BufferUtils::append_bytes(data, lobbyId.data(), lobbyId.size());
    return data;
}
