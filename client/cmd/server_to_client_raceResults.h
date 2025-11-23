#ifndef CLIENT_SERVER_TO_CLIENT_RACE_RESULTS_H
#define CLIENT_SERVER_TO_CLIENT_RACE_RESULTS_H

#include <string>
#include <vector>

#include "../../common/constants.h"

#include "cmd_base_client.h"

struct ClientPlayerResult {
    uint32_t playerId;
    std::string playerName;
    float finishTime;
    uint8_t position;
};

class ServerToClientRaceResults: public ServerToClientCmd_Client {
private:
    std::vector<ClientPlayerResult> results;
    bool isFinished;

public:
    explicit ServerToClientRaceResults(std::vector<ClientPlayerResult> playerResults = {},
                                       bool finished = false);

    static ServerToClientRaceResults from_bytes(const std::vector<uint8_t>& data);
    void execute(ClientContext& ctx) override;
};

#endif
