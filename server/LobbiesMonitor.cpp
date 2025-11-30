#include "LobbiesMonitor.h"

#include "client_handler.h"

#define MAX_PLAYERS_PER_LOBBY 8

LobbiesMonitor::LobbiesMonitor() {}

void LobbiesMonitor::killFinishedLobbies() {
    for (auto& [_, lobby]: lobbies) {
        if (lobby->status == LobbyStatus::FINISHED) {
            lobby->gameloop.empty_gameloop_queue();
            lobby->gameloop.stop();
            lobby->gameloop.join();
        }
    }
}

Queue<ClientToServerCmd_Server*>* LobbiesMonitor::createLobby(
        const std::string& lobbyId, std::shared_ptr<ServerClientHandler> client) {
    std::lock_guard<std::mutex> lock(mutex);
    killFinishedLobbies();
    auto [it, inserted] = lobbies.emplace(lobbyId, nullptr);
    if (!inserted) {
        return nullptr;
    }
    it->second = std::make_shared<Lobby>(lobbyId);
    it->second->connectedClients.add_client(client);
    it->second->lobbyStart();
    return &(it->second->gameloop_queue);
}

Queue<ClientToServerCmd_Server*>* LobbiesMonitor::joinLobby(
        std::string lobbyId, std::shared_ptr<ServerClientHandler> client) {
    std::lock_guard<std::mutex> lock(mutex);
    auto lobby = lobbies.find(lobbyId);
    if (lobby == lobbies.end()) {
        return nullptr;
    }
    if (lobby->second->status != LobbyStatus::WAITING_PLAYERS) {
        return nullptr;
    }
    if (lobby->second->connectedClients.size() >= MAX_PLAYERS_PER_LOBBY) {
        return nullptr;
    }
    lobby->second->connectedClients.add_client(client);
    return &(lobby->second->gameloop_queue);
}

void LobbiesMonitor::closeAllLobbies() {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& [_, lobby]: lobbies) {
        lobby->gameloop.stop();
        lobby->gameloop.empty_gameloop_queue();
        lobby->gameloop.join();
    }
    lobbies.clear();
}

LobbiesMonitor::~LobbiesMonitor() {}
