#include "client_to_server_readyToStart.h"

#include <iostream>
#include <stdexcept>
#include <vector>

ClientToServerReady::ClientToServerReady(int client_id): ClientToServerCmd_Server(client_id) {}

void ClientToServerReady::execute(ServerContext& ctx) {
    std::cout << "Cliente con id: " << client_id << " está listo para comenzar la carrera."
              << std::endl;
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

    return new ClientToServerReady(client_id);
}
