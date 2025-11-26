#ifndef SERVER_TO_CLIENT_JOIN_LOBBY_RESPONSE_CLIENT_H
#define SERVER_TO_CLIENT_JOIN_LOBBY_RESPONSE_CLIENT_H

#include <string>
#include <utility>
#include <vector>

#include "../../common/constants.h"

#include "cmd_base_client.h"

class ServerToClientJoinLobbyResponse_Client: public ServerToClientCmd_Client {
private:
    uint8_t status;
    std::string lobbyId;
    uint8_t errorCode;

public:
    ServerToClientJoinLobbyResponse_Client(uint8_t status, std::string lobbyId, uint8_t errorCode);

    virtual void execute(ClientContext& ctx) override;

    static ServerToClientJoinLobbyResponse_Client from_bytes(const std::vector<uint8_t>& data);

    uint8_t get_only_for_test_status() const;
    const std::string& get_only_for_test_lobbyId() const;
    uint8_t get_only_for_test_errorCode() const;
};

#endif  // SERVER_TO_CLIENT_JOIN_LOBBY_RESPONSE_CLIENT_H
