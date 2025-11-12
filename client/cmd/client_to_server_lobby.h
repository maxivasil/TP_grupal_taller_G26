#ifndef CLIENT_TO_SERVER_LOBBY_H
#define CLIENT_TO_SERVER_LOBBY_H

#include <cstring>
#include <string>
#include <vector>

#include "../../common/buffer_utils.h"
#include "../../common/constants.h"

#include "cmd_base_client.h"

class ClientToServerLobby: public ClientToServerCmd_Client {
public:
    explicit ClientToServerLobby(std::string lobbyId, bool isCreate);
    std::vector<uint8_t> to_bytes() const override;  // Implementación de serialización
private:
    std::string lobbyId;
    bool isCreate;
};

#endif 
