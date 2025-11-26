#ifndef CLIENT_TO_SERVER_JOIN_LOBBY_SERVER_H
#define CLIENT_TO_SERVER_JOIN_LOBBY_SERVER_H

#include <string>
#include <vector>

#include "../../common/constants.h"

#include "cmd_base_server.h"

class ClientToServerJoinLobby_Server: public ClientToServerCmd_Server {
public:
    explicit ClientToServerJoinLobby_Server(std::string&& lobbyId, uint8_t type,
                                            uint32_t client_id);

    void execute(ServerContext& ctx) override;

    static ClientToServerJoinLobby_Server* from_bytes(const std::vector<uint8_t>& data,
                                                      const uint32_t client_id);

    const std::string& get_only_for_test_lobbyId() const;
    uint8_t get_only_for_test_type() const;

private:
    std::string lobbyId;
    uint8_t type;
};

#endif  // CLIENT_TO_SERVER_JOIN_LOBBY_SERVER_H
