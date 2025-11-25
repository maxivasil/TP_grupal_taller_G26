#ifndef CLIENT_TO_SERVER_JOIN_LOBBY_CLIENT_H
#define CLIENT_TO_SERVER_JOIN_LOBBY_CLIENT_H

#include <cstring>
#include <string>
#include <vector>

#include "../../common/buffer_utils.h"
#include "../../common/constants.h"

#include "cmd_base_client.h"

class ClientToServerJoinLobby_Client: public ClientToServerCmd_Client {
public:
    explicit ClientToServerJoinLobby_Client(std::string lobbyId, bool isCreate);
    std::vector<uint8_t> to_bytes() const override;  // Implementación de serialización
private:
    std::string lobbyId;
    bool isCreate;
};

#endif
