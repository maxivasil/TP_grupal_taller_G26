#ifndef SERVER_TO_CLIENT_GAME_STARTING_SERVER_H
#define SERVER_TO_CLIENT_GAME_STARTING_SERVER_H

#include <vector>

#include "../../common/buffer_utils.h"
#include "../../common/constants.h"

#include "cmd_base_server.h"

class ServerToClientGameStarting_Server: public ServerToClientCmd_Server {
public:
    ServerToClientGameStarting_Server();

    std::vector<uint8_t> to_bytes() const override;

    ServerToClientCmd_Server* clone() const override;
};

#endif  // SERVER_TO_CLIENT_GAME_STARTING_SERVER_H
