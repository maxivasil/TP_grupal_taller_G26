#ifndef SERVER_TO_CLIENT_LOBBY_RESPONSE_H
#define SERVER_TO_CLIENT_LOBBY_RESPONSE_H

#include <string>
#include <utility>
#include <vector>

#include "../../common/constants.h"

#include "cmd_base_client.h"

class ServerToClientLobbyResponse: public ServerToClientCmd_Client {
private:
    uint8_t status;
    std::string lobbyId;
    uint8_t errorCode;

public:
    ServerToClientLobbyResponse(uint8_t status, std::string lobbyId, uint8_t errorCode);

    virtual void execute(Game& game) override;

    static ServerToClientLobbyResponse from_bytes(const std::vector<uint8_t>& data);
};

#endif  // SERVER_TO_CLIENT_LOBBY_RESPONSE_H
