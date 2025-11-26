#ifndef SERVER_TO_CLIENT_GAME_STARTING_CLIENT_H
#define SERVER_TO_CLIENT_GAME_STARTING_CLIENT_H

#include <string>
#include <utility>
#include <vector>

#include "../../common/constants.h"

#include "cmd_base_client.h"

class ServerToClientGameStarting_Client: public ServerToClientCmd_Client {
public:
    ServerToClientGameStarting_Client();

    virtual void execute(ClientContext& ctx) override;

    static ServerToClientGameStarting_Client from_bytes(const std::vector<uint8_t>& data);
};

#endif  // SERVER_TO_CLIENT_GAME_STARTING_CLIENT_H
