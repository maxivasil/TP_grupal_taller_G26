#ifndef SERVER_TO_CLIENT_STARTING_RACE_H
#define SERVER_TO_CLIENT_STARTING_RACE_H

#include <string>
#include <utility>
#include <vector>

#include "../../common/constants.h"

#include "cmd_base_client.h"

class ServerToClientStartingRace: public ServerToClientCmd_Client {
private:
    uint8_t cityId;

public:
    explicit ServerToClientStartingRace(uint8_t cityId);

    virtual void execute(ClientContext& ctx) override;

    static ServerToClientStartingRace from_bytes(const std::vector<uint8_t>& data);
};

#endif  // SERVER_TO_CLIENT_STARTING_RACE_H
