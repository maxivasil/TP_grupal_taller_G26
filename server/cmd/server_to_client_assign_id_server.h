#ifndef SERVER_TO_CLIENT_ASSIGNID_SERVER_H
#define SERVER_TO_CLIENT_ASSIGNID_SERVER_H

#include <cstdint>
#include <vector>

#include "../../common/constants.h"

#include "cmd_base_server.h"

class ServerToClientAssignId_Server: public ServerToClientCmd_Server {
private:
    uint32_t client_id;

public:
    explicit ServerToClientAssignId_Server(uint32_t client_id);

    std::vector<uint8_t> to_bytes() const override;

    virtual ServerToClientCmd_Server* clone() const override;
};

#endif  // SERVER_TO_CLIENT_ASSIGNID_SERVER_H
