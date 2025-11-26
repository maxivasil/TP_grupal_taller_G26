#include "client_to_server_joinLobby_client.h"

#include <string>

ClientToServerJoinLobby_Client::ClientToServerJoinLobby_Client(std::string lobbyId, bool isCreate):
        lobbyId(lobbyId), isCreate(isCreate) {}

std::vector<uint8_t> ClientToServerJoinLobby_Client::to_bytes() const {
    std::vector<uint8_t> data;
    BufferUtils::append_uint8(data, JOIN_COMMAND);
    BufferUtils::append_uint8(data, isCreate ? TYPE_CREATE : TYPE_JOIN);
    BufferUtils::append_bytes(data, lobbyId.data(), lobbyId.size());
    return data;
}
