#include "client_to_server_cheat.h"

#include <iostream>
#include <stdexcept>

#include "../game_logic/Race.h"

ClientToServerCheat::ClientToServerCheat(uint8_t cheat_type, int client_id):
        cheat_type(cheat_type), ClientToServerCmd_Server(client_id) {}

void ClientToServerCheat::execute(ServerContext& ctx) {
    std::cout << "CHEAT activado: " << static_cast<int>(cheat_type) 
              << " por cliente: " << client_id << std::endl;
    
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
ClientToServerCheat* ClientToServerCheat::from_bytes(const std::vector<uint8_t>& data,
                                                     const int client_id) {
    if (data.size() < 2) {
        throw std::runtime_error("CheatCmd: datos insuficientes");
    }

    uint8_t cheat_value = data[1];  // data[0] sería el CHEAT_COMMAND
    if (cheat_value < CHEAT_INFINITE_HEALTH || cheat_value > CHEAT_LOSE) {
        throw std::runtime_error("CheatCmd: tipo de cheat inválido");
    }

    return new ClientToServerCheat(cheat_value, client_id);
}

