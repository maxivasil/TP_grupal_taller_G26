#include "server_to_client_gameStarting.h"

ServerToClientGameStarting::ServerToClientGameStarting(){}

std::vector<uint8_t> ServerToClientGameStarting::to_bytes() const {
    std::vector<uint8_t> data;
    BufferUtils::append_uint8(data, GAME_STARTING_COMMAND);
    return data;
}

ServerToClientCmd_Server* ServerToClientGameStarting::clone() const {
    return new ServerToClientGameStarting(*this);
}
