#ifndef SERVER_TO_CLIENT_ACCUMULATEDRESULTS_H
#define SERVER_TO_CLIENT_ACCUMULATEDRESULTS_H

#include <string>
#include <vector>

#include <arpa/inet.h>

#include "../../common/constants.h"

#include "cmd_base_client.h"

struct AccumulatedResultDTO {
    uint8_t playerId;
    std::string name;
    int completedRaces;
    float totalTime;
};

class ServerToClientAccumulatedResults: public ServerToClientCmd_Client {
private:
    std::vector<AccumulatedResultDTO> results;

public:
    ServerToClientAccumulatedResults(const std::vector<AccumulatedResultDTO>& res);

    // const std::vector<AccumulatedResultDTO>& getResults() const { return results; }

    virtual void execute(ClientContext& ctx) override;

    static ServerToClientAccumulatedResults from_bytes(const std::vector<uint8_t>& data);
};

#endif  // SERVER_TO_CLIENT_ACCUMULATEDRESULTS_H
