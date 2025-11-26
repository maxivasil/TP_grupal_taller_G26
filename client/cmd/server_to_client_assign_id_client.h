#ifndef SERVER_TO_CLIENT_ASSIGNID_CLIENT_H
#define SERVER_TO_CLIENT_ASSIGNID_CLIENT_H

#include <cstdint>
#include <vector>

#include "../../common/constants.h"

#include "cmd_base_client.h"

class ServerToClientAssignId_Client: public ServerToClientCmd_Client {
private:
    uint32_t client_id;

public:
    explicit ServerToClientAssignId_Client(uint32_t client_id);

    virtual void execute(ClientContext& ctx) override;

    static ServerToClientAssignId_Client from_bytes(const std::vector<uint8_t>& data);

    uint32_t get_only_for_test_client_id() const;
};

#endif  // SERVER_TO_CLIENT_ASSIGNID_CLIENT_H
