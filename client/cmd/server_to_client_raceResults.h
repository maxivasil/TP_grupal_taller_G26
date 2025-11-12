#ifndef CLIENT_SERVER_TO_CLIENT_RACE_RESULTS_H
#define CLIENT_SERVER_TO_CLIENT_RACE_RESULTS_H

#include "cmd_base_client.h"
#include "../../common/constants.h"
#include <vector>
#include <string>

struct ClientPlayerResult {
    uint8_t playerId;
    std::string playerName;
    float finishTime;
    uint8_t position;
};

class ServerToClientRaceResults : public ServerToClientCmd_Client {
private:
    std::vector<ClientPlayerResult> results;
    bool isFinished;

public:
    explicit ServerToClientRaceResults(std::vector<ClientPlayerResult> playerResults = {}, bool finished = false);
    
    static ServerToClientRaceResults from_bytes(const std::vector<uint8_t>& data);
    void execute(ClientContext& ctx) override;
    
    const std::vector<ClientPlayerResult>& getResults() const { return results; }
    bool getRaceFinished() const { return isFinished; }
};

#endif  // CLIENT_SERVER_TO_CLIENT_RACE_RESULTS_H
