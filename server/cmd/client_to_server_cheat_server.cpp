#include "client_to_server_cheat_server.h"

#include <iostream>
#include <stdexcept>

#include "../game_logic/Race.h"

ClientToServerCheat_Server::ClientToServerCheat_Server(uint8_t cheat_type, uint32_t client_id):
        cheat_type(cheat_type), ClientToServerCmd_Server(client_id) {}

void ClientToServerCheat_Server::execute(ServerContext& ctx) {
    std::cout << "CHEAT activado: " << static_cast<int>(cheat_type) << " por cliente: " << client_id
              << std::endl;

    if (!ctx.inLobby || !*(ctx.inLobby) || !ctx.race) {
        return;
    }

    switch (cheat_type) {
        case CHEAT_INFINITE_HEALTH: {
            ctx.race->activateInfiniteHealthCheat(client_id);
            break;
        }
        case CHEAT_WIN: {
            ctx.race->forceWinCheat(client_id);
            break;
        }
        case CHEAT_LOSE: {
            ctx.race->forceLoseCheat(client_id);
            break;
        }
        default:
            std::cerr << "Cheat inválido: " << static_cast<int>(cheat_type) << std::endl;
            break;
    }
}

// Deserialización desde bytes
ClientToServerCheat_Server* ClientToServerCheat_Server::from_bytes(const std::vector<uint8_t>& data,
                                                                   const uint32_t client_id) {
    if (data.size() < 2) {
        throw std::runtime_error("CheatCmd: datos insuficientes");
    }

    uint8_t cheat_value = data[1];
    if (cheat_value < CHEAT_INFINITE_HEALTH || cheat_value > CHEAT_LOSE) {
        throw std::runtime_error("CheatCmd: tipo de cheat inválido");
    }

    return new ClientToServerCheat_Server(cheat_value, client_id);
}

uint8_t ClientToServerCheat_Server::get_only_for_test_cheat_type() const { return cheat_type; }
