#ifndef CLIENT_TO_SERVER_FINISH_RACE_H
#define CLIENT_TO_SERVER_FINISH_RACE_H

#include <vector>
#include "../../common/constants.h"
#include "cmd_base_server.h"

class ClientToServerFinishRace : public ClientToServerCmd_Server {
public:
    ClientToServerFinishRace(int client_id);
    
    static ClientToServerFinishRace* from_bytes(const std::vector<uint8_t>& data, int client_id);
    void execute(ServerContext& ctx) override;

private:
    int client_id;
};

#endif  // CLIENT_TO_SERVER_FINISH_RACE_H
