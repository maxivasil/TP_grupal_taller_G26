#ifndef SERVER_TO_CLIENT_RACE_RESULTS_SERVER_H
#define SERVER_TO_CLIENT_RACE_RESULTS_SERVER_H

#include <string>
#include <vector>

#include "../../common/buffer_utils.h"
#include "../../common/constants.h"

#include "cmd_base_server.h"

struct PlayerResult {
    uint32_t playerId;
    std::string playerName;
    float finishTime;
    uint8_t position;
};

class ServerToClientRaceResults_Server: public ServerToClientCmd_Server {
private:
    std::vector<PlayerResult> results;
    bool isFinished;

public:
    ServerToClientRaceResults_Server(const std::vector<PlayerResult>& playerResults, bool finished);

    std::vector<uint8_t> to_bytes() const override;
    ServerToClientCmd_Server* clone() const override;
};

#endif  // SERVER_TO_CLIENT_RACE_RESULTS_SERVER_H
