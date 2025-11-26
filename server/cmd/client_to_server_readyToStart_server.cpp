#include "client_to_server_readyToStart_server.h"

#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

ClientToServerReady_Server::ClientToServerReady_Server(uint32_t client_id, const std::string& car,
                                                       const std::string& username):
        ClientToServerCmd_Server(client_id), car(car), username(username) {}

void ClientToServerReady_Server::execute(ServerContext& ctx) {
    std::cout << "Cliente con id: " << client_id
              << " está listo para comenzar la carrera con el auto " << car << std::endl;

    // Store the selected car in the lobby
    if (ctx.lobby) {
        ctx.lobby->clientCarSelection[client_id] = car;
        ctx.lobby->clientUsernames[client_id] = username;
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
    if (data.size() < 2) {
        throw std::runtime_error("ReadyToStartCmd: datos insuficientes");
    }
    uint8_t len = data[1];
    std::string car(data.begin() + 2, data.begin() + 2 + len);
    std::string username(data.begin() + 2 + len, data.end());

    return new ClientToServerReady_Server(client_id, car, username);
}

const std::string& ClientToServerReady_Server::get_only_for_test_car() const { return car; }
