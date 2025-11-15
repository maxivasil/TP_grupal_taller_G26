#ifndef SERVER_TO_CLIENT_ACCUMULATEDRESULTS_H
#define SERVER_TO_CLIENT_ACCUMULATEDRESULTS_H

#include <cstdint>
#include <vector>

#include "../../common/buffer_utils.h"
#include "../../common/constants.h"

#include "cmd_base_server.h"

struct AccumulatedInfo {
    int completedRaces = 0;
    float totalTime = 0.0f;
};

class ServerToClientAccumulatedResults: public ServerToClientCmd_Server {
private:
    std::unordered_map<int, AccumulatedInfo> accumulatedResults;

public:
    explicit ServerToClientAccumulatedResults(
            std::unordered_map<int, AccumulatedInfo> accumulatedResults);

    std::vector<uint8_t> to_bytes() const override;

    virtual ServerToClientCmd_Server* clone() const override;
};

#endif  // SERVER_TO_CLIENT_ACCUMULATEDRESULTS_H
