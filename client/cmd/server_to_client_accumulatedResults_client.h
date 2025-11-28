#ifndef SERVER_TO_CLIENT_ACCUMULATEDRESULTS_CLIENT_H
#define SERVER_TO_CLIENT_ACCUMULATEDRESULTS_CLIENT_H

#include <string>
#include <vector>

#include <arpa/inet.h>

#include "../../common/constants.h"

#include "cmd_base_client.h"

struct AccumulatedResultDTO {
    uint32_t playerId;
    uint16_t completedRaces;
    float totalTime;
};

class ServerToClientAccumulatedResults_Client: public ServerToClientCmd_Client {
private:
    std::vector<AccumulatedResultDTO> results;
    bool isLastRace;

public:
    ServerToClientAccumulatedResults_Client(const std::vector<AccumulatedResultDTO>& res,
                                            bool isLastRace);

    virtual void execute(ClientContext& ctx) override;

    static ServerToClientAccumulatedResults_Client from_bytes(const std::vector<uint8_t>& data);

    const std::vector<AccumulatedResultDTO> get_only_for_test_results() const;

    bool get_only_for_test_isLastRace() const;
};

#endif  // SERVER_TO_CLIENT_ACCUMULATEDRESULTS_CLIENT_H
