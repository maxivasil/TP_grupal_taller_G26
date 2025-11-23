#ifndef SERVER_TO_CLIENT_ASSIGNID_H
#define SERVER_TO_CLIENT_ASSIGNID_H

#include <cstdint>
#include <vector>

#include "../../common/constants.h"

#include "cmd_base_client.h"

class ServerToClientAssignId: public ServerToClientCmd_Client {
private:
    uint32_t client_id;

public:
    explicit ServerToClientAssignId(uint32_t client_id);

    virtual void execute(ClientContext& ctx) override;

    static ServerToClientAssignId from_bytes(const std::vector<uint8_t>& data);
};

#endif  // SERVER_TO_CLIENT_ASSIGNID_H
