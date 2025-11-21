#ifndef SERVER_TO_CLIENT_GAME_STARTING_H
#define SERVER_TO_CLIENT_GAME_STARTING_H

#include <string>
#include <utility>
#include <vector>

#include "../../common/constants.h"

#include "cmd_base_client.h"

class ServerToClientGameStarting: public ServerToClientCmd_Client {
public:
    ServerToClientGameStarting();

    virtual void execute(ClientContext& ctx) override;

    static ServerToClientGameStarting from_bytes(const std::vector<uint8_t>& data);
};

#endif  // SERVER_TO_CLIENT_GAME_STARTING_H
