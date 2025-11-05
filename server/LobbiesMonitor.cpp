#include "LobbiesMonitor.h"


LobbiesMonitor::LobbiesMonitor() {}

void LobbiesMonitor::killFinishedLobbies() {
    for (auto& [_, lobby]: lobbies) {
        if (lobby->status == LobbyStatus::FINISHED) {
            lobby->connectedClients.stop_all_and_delete();
            lobby->gameloop_queue->close();
            lobby->gameloop.stop();
        }
    }
}
#include <iostream>
bool LobbiesMonitor::createLobby(const std::string& lobbyId) {
    std::cout << "EJECUTANDO CREATE LOBBY" << std::endl;
    std::lock_guard<std::mutex> lock(mutex);
    killFinishedLobbies();
    auto [it, inserted] = lobbies.emplace(lobbyId, nullptr);
    if (!inserted) {
        return false;
    }
    it->second = std::make_shared<Lobby>(lobbyId);
    return true;
}

Queue<ClientToServerCmd_Server*>* LobbiesMonitor::joinLobby(std::string lobbyId,
                                                            ServerClientHandler* client) {
    std::cout << "EJECUTANDO JOIN LOBBY" << std::endl;
    std::lock_guard<std::mutex> lock(mutex);
    auto lobby = lobbies.find(lobbyId);
    if (lobby == lobbies.end()) {
        return nullptr;
    }
    if (lobby->second->status != LobbyStatus::WAITING_PLAYERS) {
        return nullptr;
    }
    lobby->second->connectedClients.add_client(client);
    return lobby->second->gameloop_queue.get();
}

void LobbiesMonitor::closeAllLobbies() {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& [_, lobby]: lobbies) {
        lobby->gameloop.stop();
        lobby->connectedClients.stop_all_and_delete();
        lobby->gameloop_queue->close();
        lobby->gameloop.join();
    }
}

LobbiesMonitor::~LobbiesMonitor() {}
