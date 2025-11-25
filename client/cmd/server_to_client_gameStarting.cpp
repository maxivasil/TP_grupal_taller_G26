#include "server_to_client_gameStarting.h"

#include <cstring>
#include <stdexcept>

#include <arpa/inet.h>

#include "../UI/mainwindow.h"

ServerToClientGameStarting::ServerToClientGameStarting() {}


void ServerToClientGameStarting::execute(ClientContext& ctx) { ctx.mainwindow->close(); }

ServerToClientGameStarting ServerToClientGameStarting::from_bytes(
        const std::vector<uint8_t>& data) {
    if (data.empty()) {
        throw std::runtime_error("Game Starting data is empty");
    }
    uint8_t header = data[0];
    if (header != GAME_STARTING_COMMAND)
        throw std::runtime_error("Invalid header for Game Starting");
    return ServerToClientGameStarting();
}
