#include "client_to_server_readyToStart_server.h"

#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

ClientToServerReady_Server::ClientToServerReady_Server(uint32_t client_id, std::string car):
        ClientToServerCmd_Server(client_id), car(std::move(car)) {}

void ClientToServerReady_Server::execute(ServerContext& ctx) {
    std::cout << "Cliente con id: " << client_id
              << " está listo para comenzar la carrera con el auto " << car << std::endl;

    // Store the selected car in the lobby
    if (ctx.lobby) {
        ctx.lobby->clientCarSelection[client_id] = car;
        std::cout << "Auto guardado para cliente " << client_id << ": " << car << std::endl;
    }

    if (!ctx.inLobby || !*(ctx.inLobby) || !ctx.clientsReady) {
        return;
    }
    std::cout << "Cliente con id: " << client_id
              << " está listo para comenzar la carrera con el auto " << car << std::endl;
    ctx.clientsReady->insert(client_id);
}

// Deserialización desde bytes
ClientToServerReady_Server* ClientToServerReady_Server::from_bytes(const std::vector<uint8_t>& data,
                                                                   const uint32_t client_id) {
    if (data.size() < 1) {
        throw std::runtime_error("ReadyToStartCmd: datos insuficientes");
    }
    std::string car(data.begin() + 1, data.end());

    return new ClientToServerReady_Server(client_id, car);
}

const std::string& ClientToServerReady_Server::get_only_for_test_car() const { return car; }
