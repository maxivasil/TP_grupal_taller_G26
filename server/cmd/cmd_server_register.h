#ifndef SERVER_CMD_REGISTER_H
#define SERVER_CMD_REGISTER_H

#include <unordered_map>
#include <vector>

#include "../../common/constants.h"

#include "client_to_server_move.h"
#include "cmd_base_server.h"

class ServerRegisteredCommands {
public:
    ServerRegisteredCommands();
    const std::unordered_map<uint8_t,
                             std::function<ServerToClientCmd_Server*(const std::vector<uint8_t>&)>>&
            get_send_registry() const;
    const std::unordered_map<uint8_t,
                             std::function<ClientToServerCmd_Server*(const std::vector<uint8_t>&)>>&
            get_recv_registry() const;

private:
    std::unordered_map<uint8_t,
                       std::function<ServerToClientCmd_Server*(const std::vector<uint8_t>&)>>
            send_registry;
    std::unordered_map<uint8_t,
                       std::function<ClientToServerCmd_Server*(const std::vector<uint8_t>&)>>
            recv_registry;

    std::unordered_map<uint8_t,
                       std::function<ServerToClientCmd_Server*(const std::vector<uint8_t>&)>>
            build_server_send_command_registry();
    std::unordered_map<uint8_t,
                       std::function<ClientToServerCmd_Server*(const std::vector<uint8_t>&)>>
            build_server_recv_command_registry();
};

#endif  // SERVER_CMD_REGISTER_H
