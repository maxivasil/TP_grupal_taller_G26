#ifndef SERVER_TO_CLIENT_ACCUMULATEDRESULTS_SERVER_H
#define SERVER_TO_CLIENT_ACCUMULATEDRESULTS_SERVER_H

#include <cstdint>
#include <utility>
#include <vector>

#include "../../common/buffer_utils.h"
#include "../../common/constants.h"

#include "cmd_base_server.h"

struct AccumulatedResultDTO {
    uint32_t playerId;
    uint16_t completedRaces;
    float totalTime;
};

class ServerToClientAccumulatedResults_Server: public ServerToClientCmd_Server {
private:
    std::vector<AccumulatedResultDTO> results;

public:
    explicit ServerToClientAccumulatedResults_Server(std::vector<AccumulatedResultDTO> results);

    std::vector<uint8_t> to_bytes() const override;

    virtual ServerToClientCmd_Server* clone() const override;
};

#endif  // SERVER_TO_CLIENT_ACCUMULATEDRESULTS_SERVER_H
