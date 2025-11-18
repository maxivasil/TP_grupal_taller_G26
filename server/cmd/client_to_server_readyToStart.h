#ifndef CLIENT_TO_SERVER_READY_TO_START_H
#define CLIENT_TO_SERVER_READY_TO_START_H

#include <vector>
#include <string>

#include "../../common/constants.h"
#include "../Lobby.h"

#include "cmd_base_server.h"

class ClientToServerReady: public ClientToServerCmd_Server {
public:
    explicit ClientToServerReady(int client_id, std::string car);

    void execute(ServerContext& ctx) override;

    static ClientToServerReady* from_bytes(const std::vector<uint8_t>& data, const int client_id);
private:
    std::string car;
};

#endif  // CLIENT_TO_SERVER_READY_TO_START_H
