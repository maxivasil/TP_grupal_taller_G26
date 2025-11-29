#ifndef SERVER_TO_CLIENT_STARTING_RACE_SERVER_H
#define SERVER_TO_CLIENT_STARTING_RACE_SERVER_H

#include <string>
#include <vector>

#include "../../common/buffer_utils.h"
#include "../../common/constants.h"

#include "cmd_base_server.h"

class ServerToClientStartingRace_Server: public ServerToClientCmd_Server {
private:
    uint8_t cityId;
    std::string trackFile;
    bool isLastRace;

public:
    ServerToClientStartingRace_Server(uint8_t cityId, std::string& trackFile, bool isLastRace);

    std::vector<uint8_t> to_bytes() const override;

    ServerToClientCmd_Server* clone() const override;
};

#endif  // SERVER_TO_CLIENT_STARTING_RACE_SERVER_H
