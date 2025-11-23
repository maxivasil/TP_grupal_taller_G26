#ifndef SERVER_TO_CLIENT_ACCUMULATEDRESULTS_H
#define SERVER_TO_CLIENT_ACCUMULATEDRESULTS_H

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

class ServerToClientAccumulatedResults: public ServerToClientCmd_Client {
private:
    std::vector<AccumulatedResultDTO> results;

public:
    explicit ServerToClientAccumulatedResults(const std::vector<AccumulatedResultDTO>& res);

    virtual void execute(ClientContext& ctx) override;

    static ServerToClientAccumulatedResults from_bytes(const std::vector<uint8_t>& data);
};

#endif  // SERVER_TO_CLIENT_ACCUMULATEDRESULTS_H
