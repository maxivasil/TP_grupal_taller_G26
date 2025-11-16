#ifndef SERVER_TO_CLIENT_STARTING_RACE_H
#define SERVER_TO_CLIENT_STARTING_RACE_H

#include <vector>

#include "../../common/buffer_utils.h"
#include "../../common/constants.h"

#include "cmd_base_server.h"

class ServerToClientStartingRace: public ServerToClientCmd_Server {
private:
    uint8_t cityId;

public:
    explicit ServerToClientStartingRace(uint8_t cityId);

    std::vector<uint8_t> to_bytes() const override;

    ServerToClientCmd_Server* clone() const override;
};

#endif  // SERVER_TO_CLIENT_STARTING_RACE_H
