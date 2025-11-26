#include "server_to_client_gameStarting_server.h"

ServerToClientGameStarting_Server::ServerToClientGameStarting_Server() {}

std::vector<uint8_t> ServerToClientGameStarting_Server::to_bytes() const {
    std::vector<uint8_t> data;
    BufferUtils::append_uint8(data, GAME_STARTING_COMMAND);
    return data;
}

ServerToClientCmd_Server* ServerToClientGameStarting_Server::clone() const {
    return new ServerToClientGameStarting_Server(*this);
}
