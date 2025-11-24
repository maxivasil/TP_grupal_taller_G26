#include "client_to_server_move.h"

#include <iostream>
#include <stdexcept>
#include <vector>

#include "../game_logic/Race.h"

ClientToServerMove::ClientToServerMove(uint8_t direction, uint32_t client_id):
        direction(direction), ClientToServerCmd_Server(client_id) {}

void ClientToServerMove::execute(ServerContext& ctx) {
    std::cout << "Ejecutando movimiento en dirección: " << static_cast<int>(direction)
              << " del cliente con id: " << client_id << std::endl;
    if (!ctx.inLobby || !*(ctx.inLobby) || !ctx.race)
        return;
    switch (direction) {
        case MOVE_UP:
            ctx.race->acceleratePlayer(client_id);
            break;
        case MOVE_DOWN:
            ctx.race->brakePlayer(client_id);
            break;
        case MOVE_LEFT:
            ctx.race->turnPlayer(client_id, Direction::LEFT);
            break;
        case MOVE_RIGHT:
            ctx.race->turnPlayer(client_id, Direction::RIGHT);
            break;
        default:
            break;
    }
}

// Deserialización desde bytes
ClientToServerMove* ClientToServerMove::from_bytes(const std::vector<uint8_t>& data,
                                                   const uint32_t client_id) {
    if (data.size() < 2) {
        throw std::runtime_error("MoveCmd: datos insuficientes");
    }

    uint8_t dir_value = data[1];
    if (dir_value > MOVE_RIGHT) {
        throw std::runtime_error("MoveCmd: dirección inválida");
    }

    return new ClientToServerMove(dir_value, client_id);
}
