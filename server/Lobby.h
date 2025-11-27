#ifndef LOBBY_H
#define LOBBY_H

#include <memory>
#include <string>
#include <unordered_map>

#include "LobbyStatus.h"
#include "gameloop.h"
#include "protected_clients.h"

struct Lobby {
    std::string lobbyId;
    ServerProtectedClients connectedClients;
    LobbyStatus status;
    Queue<ClientToServerCmd_Server*> gameloop_queue;
    ServerGameLoop gameloop;
    // Map to store selected car for each client: client_id -> car_name
    std::unordered_map<int, std::string> clientCarSelection;
    std::unordered_map<int, std::string> clientUsernames;

    explicit Lobby(std::string lobbyId):
            lobbyId(lobbyId),
            status(LobbyStatus::WAITING_PLAYERS),
            gameloop_queue(UINT32_MAX),
            gameloop(gameloop_queue, connectedClients, status, this) {}
    ~Lobby() {}
    void lobbyStart() { gameloop.start(); }
};


#endif
