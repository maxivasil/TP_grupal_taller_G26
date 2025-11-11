#ifndef CLIENT_TO_SERVER_FINISH_RACE_H
#define CLIENT_TO_SERVER_FINISH_RACE_H

#include <vector>
#include "../../common/constants.h"
#include "cmd_base_client.h"

class ClientToServerFinishRace : public ClientToServerCmd_Client {
public:
    ClientToServerFinishRace();
    std::vector<uint8_t> to_bytes() const override;
};

#endif  // CLIENT_TO_SERVER_FINISH_RACE_H
