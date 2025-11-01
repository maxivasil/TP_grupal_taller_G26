#ifndef CLIENT_CMD_REGISTER_H
#define CLIENT_CMD_REGISTER_H

#include <unordered_map>
#include <utility>
#include <vector>

#include "client_to_server_move.h"
#include "cmd_base_client.h"
#include "server_to_client_snapshot.h"

class ClientRegisteredCommands {
public:
    ClientRegisteredCommands();
    const std::unordered_map<uint8_t,
                             std::function<ServerToClientCmd_Client*(const std::vector<uint8_t>&)>>&
            get_recv_registry() const;

private:
    std::unordered_map<uint8_t,
                       std::function<ServerToClientCmd_Client*(const std::vector<uint8_t>&)>>
            recv_registry;

    std::unordered_map<uint8_t,
                       std::function<ServerToClientCmd_Client*(const std::vector<uint8_t>&)>>
            build_client_recv_command_registry();
};

#endif  // CLIENT_CMD_REGISTER_H
