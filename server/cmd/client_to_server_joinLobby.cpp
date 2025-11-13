#include "client_to_server_joinLobby.h"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>


#include "../client_handler.h"

#include "server_to_client_joinLobbyResponse.h"

/**
 * <HEADER>;<TYPE>;<LOBBY_ID>
 * LOBBY_ID -> len 6 string
 * TYPE 0 -> Join to a Lobby
 * TYPE 1 -> Create a Lobby
 */

ClientToServerJoinLobby::ClientToServerJoinLobby(std::string&& lobbyId, uint8_t type,
                                                 int client_id):
        lobbyId(std::move(lobbyId)), ClientToServerCmd_Server(client_id), type(type) {}

void ClientToServerJoinLobby::execute(ServerContext& ctx) {
    std::cout << "EJECUTANDO JOIN LOBBY " << lobbyId << " " << type << std::endl;
    if (ctx.inLobby && *(ctx.inLobby)) {
        std::cout << "YA en lobby " << std::endl;
        return;
    }
    if (type == TYPE_CREATE) {
        do{
            lobbyId.clear();
            for (int i = 0; i < 6; ++i) {
                lobbyId += static_cast<char>('A' + std::rand() % 26);
            }
        }while(!ctx.client->createLobby(lobbyId)); 
    }
    auto gameloop_queue = ctx.client->joinLobby(lobbyId);
    if (!gameloop_queue) {
        std::cout << "Error al conectarse al Lobby " << std::endl;
        auto error_cmd =
                std::make_shared<ServerToClientJoinResponse>(STATUS_ERROR, LOBBY_ALREADY_STARTED);
        ctx.client->send_message(error_cmd);
        return;
    }
    ctx.client->initReceiver(*gameloop_queue);
    *(ctx.inLobby) = true;
    auto success_cmd = std::make_shared<ServerToClientJoinResponse>(STATUS_OK, lobbyId);
    ctx.client->send_message(success_cmd);
    std::cout << "lobby Correctamente " << std::endl;
}

// Deserialización desde bytes
ClientToServerJoinLobby* ClientToServerJoinLobby::from_bytes(const std::vector<uint8_t>& data,
                                                             const int client_id) {
    if (data.size() < 8) {
        throw std::runtime_error("JoinLobbyCmd: datos insuficientes");
    }

    uint8_t type = data[1];  // data[0] sería el JOIN_COMMAND
    if (type != TYPE_CREATE && type != TYPE_JOIN) {
        throw std::runtime_error("JoinLobbyCmd: tipo inválido");
    }
    std::string id(data.begin() + 2, data.begin() + 8);

    return new ClientToServerJoinLobby(std::move(id), type, client_id);
}
