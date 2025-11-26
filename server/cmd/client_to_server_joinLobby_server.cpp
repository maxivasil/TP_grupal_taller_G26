#include "client_to_server_joinLobby_server.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../client_handler.h"

#include "server_to_client_joinLobbyResponse_server.h"

/**
 * <HEADER>;<TYPE>;<LOBBY_ID>
 * LOBBY_ID -> len 6 string
 * TYPE 0 -> Join to a Lobby
 * TYPE 1 -> Create a Lobby
 */

ClientToServerJoinLobby_Server::ClientToServerJoinLobby_Server(std::string&& lobbyId, uint8_t type,
                                                               uint32_t client_id):
        lobbyId(std::move(lobbyId)), ClientToServerCmd_Server(client_id), type(type) {}

void ClientToServerJoinLobby_Server::execute(ServerContext& ctx) {
    std::cout << "EJECUTANDO JOIN LOBBY " << lobbyId << " " << type << std::endl;
    if (ctx.inLobby && *(ctx.inLobby)) {
        std::cout << "YA en lobby " << std::endl;
        return;
    }
    Queue<ClientToServerCmd_Server*>* gameloop_queue = nullptr;
    if (type == TYPE_CREATE) {
        do {
            lobbyId.clear();
            for (int i = 0; i < 6; ++i) {
                lobbyId += static_cast<char>('A' + std::rand() % 26);
            }
        } while (!(gameloop_queue = ctx.client->createLobby(lobbyId)));
    }
    if (type == TYPE_JOIN) {
        gameloop_queue = ctx.client->joinLobby(lobbyId);
        if (!gameloop_queue) {
            std::cout << "Error al conectarse al Lobby " << std::endl;
            auto error_cmd = std::make_shared<ServerToClientJoinLobbyResponse_Server>(
                    STATUS_ERROR, LOBBY_ALREADY_STARTED);
            ctx.client->send_message(error_cmd);
            return;
        }
    }
    ctx.client->initReceiver(*gameloop_queue);
    *(ctx.inLobby) = true;
    auto success_cmd = std::make_shared<ServerToClientJoinLobbyResponse_Server>(STATUS_OK, lobbyId);
    ctx.client->send_message(success_cmd);
    std::cout << "lobby Correctamente " << std::endl;
}

// Deserialización desde bytes
ClientToServerJoinLobby_Server* ClientToServerJoinLobby_Server::from_bytes(
        const std::vector<uint8_t>& data, const uint32_t client_id) {
    if (data.size() < 8) {
        throw std::runtime_error("JoinLobbyCmd: datos insuficientes");
    }

    uint8_t type = data[1];  // data[0] sería el JOIN_COMMAND
    if (type != TYPE_CREATE && type != TYPE_JOIN) {
        throw std::runtime_error("JoinLobbyCmd: tipo inválido");
    }
    std::string id(data.begin() + 2, data.begin() + 8);

    return new ClientToServerJoinLobby_Server(std::move(id), type, client_id);
}

const std::string& ClientToServerJoinLobby_Server::get_only_for_test_lobbyId() const {
    return lobbyId;
}

uint8_t ClientToServerJoinLobby_Server::get_only_for_test_type() const { return type; }
