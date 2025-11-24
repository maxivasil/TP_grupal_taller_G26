#ifndef CLIENT_TO_SERVER_JOIN_LOBBY_H
#define CLIENT_TO_SERVER_JOIN_LOBBY_H

#include <string>
#include <vector>

#include "../../common/constants.h"

#include "cmd_base_server.h"

class ClientToServerJoinLobby: public ClientToServerCmd_Server {
public:
    explicit ClientToServerJoinLobby(std::string&& lobbyId, uint8_t type, uint32_t client_id);

    void execute(ServerContext& ctx) override;

    static ClientToServerJoinLobby* from_bytes(const std::vector<uint8_t>& data,
                                               const uint32_t client_id);

private:
    std::string lobbyId;
    uint8_t type;
};

#endif  // CLIENT_TO_SERVER_JOIN_LOBBY_H
