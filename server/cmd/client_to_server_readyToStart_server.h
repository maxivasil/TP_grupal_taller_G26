#ifndef CLIENT_TO_SERVER_READY_TO_START_SERVER_H
#define CLIENT_TO_SERVER_READY_TO_START_SERVER_H

#include <string>
#include <vector>

#include "../../common/constants.h"
#include "../Lobby.h"

#include "cmd_base_server.h"

class ClientToServerReady_Server: public ClientToServerCmd_Server {
public:
    ClientToServerReady_Server(uint32_t client_id, const std::string& car,
                               const std::string& username);

    void execute(ServerContext& ctx) override;

    static ClientToServerReady_Server* from_bytes(const std::vector<uint8_t>& data,
                                                  const uint32_t client_id);

    const std::string& get_only_for_test_car() const;

private:
    std::string car;
    std::string username;
};

#endif  // CLIENT_TO_SERVER_READY_TO_START_SERVER_H
