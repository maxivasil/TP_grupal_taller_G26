#include "client_to_server_move.h"

#include <iostream>
#include <stdexcept>
#include <vector>

ClientToServerMove::ClientToServerMove(uint8_t direction): direction(direction) {}

void ClientToServerMove::execute() {
    std::cout << "Ejecutando movimiento en dirección: " << static_cast<int>(direction) << std::endl;
}

// Deserialización desde bytes
ClientToServerMove* ClientToServerMove::from_bytes(const std::vector<uint8_t>& data) {
    if (data.size() < 2) {
        throw std::runtime_error("MoveCmd: datos insuficientes");
    }

    uint8_t dir_value = data[1];  // data[0] sería el MOVE_COMMAND
    if (dir_value > MOVE_RIGHT) {
        throw std::runtime_error("MoveCmd: dirección inválida");
    }

    uint8_t dir = static_cast<uint8_t>(dir_value);
    return new ClientToServerMove(dir);
}
