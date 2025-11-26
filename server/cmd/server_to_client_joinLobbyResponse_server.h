#ifndef SERVER_TO_CLIENT_JOIN_LOBBY_RESPONSE_SERVER_H
#define SERVER_TO_CLIENT_JOIN_LOBBY_RESPONSE_SERVER_H

#include <string>
#include <vector>

#include "../../common/buffer_utils.h"
#include "../../common/constants.h"

#include "cmd_base_server.h"

class ServerToClientJoinLobbyResponse_Server: public ServerToClientCmd_Server {
private:
    uint8_t status;
    std::string lobbyId;
    uint8_t errorCode;

public:
    ServerToClientJoinLobbyResponse_Server(uint8_t status, const std::string& lobbyId);
    ServerToClientJoinLobbyResponse_Server(uint8_t status, uint8_t errorCode);

    std::vector<uint8_t> to_bytes() const override;

    ServerToClientJoinLobbyResponse_Server* clone() const override;
};

#endif  // SERVER_TO_CLIENT_JOIN_LOBBY_RESPONSE_SERVER_H
