#ifndef SERVER_TO_CLIENT_RACE_RESULTS_CLIENT_H
#define SERVER_TO_CLIENT_RACE_RESULTS_CLIENT_H

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

class ServerToClientRaceResults_Client: public ServerToClientCmd_Client {
private:
    std::vector<ClientPlayerResult> results;
    bool isFinished;

public:
    explicit ServerToClientRaceResults_Client(std::vector<ClientPlayerResult> playerResults = {},
                                              bool finished = false);

    static ServerToClientRaceResults_Client from_bytes(const std::vector<uint8_t>& data);
    void execute(ClientContext& ctx) override;

    const std::vector<ClientPlayerResult>& get_only_for_test_results() const;
    bool get_only_for_test_isFinished() const;
};

#endif
