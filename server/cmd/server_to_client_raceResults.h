#ifndef SERVER_TO_CLIENT_RACE_RESULTS_H
#define SERVER_TO_CLIENT_RACE_RESULTS_H

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

    PlayerResult(): playerId(0), playerName(""), finishTime(0.0f), position(0) {}
    PlayerResult(uint32_t id, const std::string& name, float time, uint8_t pos):
            playerId(id), playerName(name), finishTime(time), position(pos) {}
};

class ServerToClientRaceResults: public ServerToClientCmd_Server {
private:
    std::vector<PlayerResult> results;
    bool isFinished;

public:
    ServerToClientRaceResults();
    ServerToClientRaceResults(const std::vector<PlayerResult>& playerResults, bool finished);

    std::vector<uint8_t> to_bytes() const override;
    ServerToClientCmd_Server* clone() const override;
};

#endif  // SERVER_TO_CLIENT_RACE_RESULTS_H
