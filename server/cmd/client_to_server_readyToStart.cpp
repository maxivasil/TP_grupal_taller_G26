#include "client_to_server_readyToStart.h"

#include <iostream>
#include <stdexcept>
#include <vector>

ClientToServerReady::ClientToServerReady(int client_id, std::string car): ClientToServerCmd_Server(client_id), car(car) {}

void ClientToServerReady::execute(ServerContext& ctx) {
    std::cout << "Cliente con id: " << client_id << " está listo para comenzar la carrera con el auto " << car
              << std::endl;
    
    // Store the selected car in the lobby
    if (ctx.lobby) {
        ctx.lobby->clientCarSelection[client_id] = car;
        std::cout << "Auto guardado para cliente " << client_id << ": " << car << std::endl;
    }
    
    if (!ctx.inLobby || !*(ctx.inLobby) || !ctx.clientsReady) {
        return;
    }
    ctx.clientsReady->insert(client_id);
}

// Deserialización desde bytes
ClientToServerReady* ClientToServerReady::from_bytes(const std::vector<uint8_t>& data,
                                                     const int client_id) {
    if (data.size() < 1) {
        throw std::runtime_error("ReadyToStartCmd: datos insuficientes");
    }
    std::string car(data.begin() + 1, data.end());

    return new ClientToServerReady(client_id, car);
}
