#include "server_to_client_gameStarting.h"

ServerToClientGameStarting::ServerToClientGameStarting() {}

std::vector<uint8_t> ServerToClientGameStarting::to_bytes() const {
    std::vector<uint8_t> data;
    uint8_t header = GAME_STARTING_COMMAND;
    data.push_back(header);
    return data;
}

ServerToClientCmd_Server* ServerToClientGameStarting::clone() const {
    return new ServerToClientGameStarting(*this);
}
