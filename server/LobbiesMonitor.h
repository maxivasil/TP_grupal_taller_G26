#ifndef LOBBIES_MONITOR_H
#define LOBBIES_MONITOR_H

#include <map>
#include <memory>
#include <string>

#include "Lobby.h"

class LobbiesMonitor {
private:
    std::mutex mutex;
    std::map<std::string, std::shared_ptr<Lobby>> lobbies;

    void killFinishedLobbies();

public:
    LobbiesMonitor();
    ~LobbiesMonitor();

    Queue<ClientToServerCmd_Server*>* createLobby(const std::string& lobbyId, ServerClientHandler* client);

    Queue<ClientToServerCmd_Server*>* joinLobby(std::string lobbyId, ServerClientHandler* client);

    void closeAllLobbies();
};


#endif
