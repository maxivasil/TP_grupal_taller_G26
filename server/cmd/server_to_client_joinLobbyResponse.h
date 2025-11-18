#ifndef SERVER_TO_CLIENT_JOIN_RESPONSE_H
#define SERVER_TO_CLIENT_JOIN_RESPONSE_H

#include <string>
#include <vector>

#include "../../common/buffer_utils.h"
#include "../../common/constants.h"

#include "cmd_base_server.h"

class ServerToClientJoinResponse: public ServerToClientCmd_Server {
private:
    uint8_t status;
    std::string lobbyId;
    uint8_t errorCode;

public:
    ServerToClientJoinResponse(uint8_t status, const std::string& lobbyId);
    ServerToClientJoinResponse(uint8_t status, uint8_t errorCode);

    std::vector<uint8_t> to_bytes() const override;

    ServerToClientJoinResponse* clone() const override;
};

#endif  // SERVER_TO_CLIENT_JOIN_RESPONSE_H
