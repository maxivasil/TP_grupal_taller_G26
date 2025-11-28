#ifndef SERVER_TO_CLIENT_STARTING_RACE_CLIENT_H
#define SERVER_TO_CLIENT_STARTING_RACE_CLIENT_H

#include <string>
#include <utility>
#include <vector>

#include "../../common/constants.h"

#include "cmd_base_client.h"

class ServerToClientStartingRace_Client: public ServerToClientCmd_Client {
private:
    uint8_t cityId;
    std::string trackFile;
    bool isLastRace;

public:
    ServerToClientStartingRace_Client(uint8_t cityId, std::string& trackFile, bool isLastRace = false);

    virtual void execute(ClientContext& ctx) override;

    static ServerToClientStartingRace_Client from_bytes(const std::vector<uint8_t>& data);
    uint8_t get_only_for_test_cityId() const;
    const std::string& get_only_for_test_trackFile() const;
};

#endif  // SERVER_TO_CLIENT_STARTING_RACE_CLIENT_H
