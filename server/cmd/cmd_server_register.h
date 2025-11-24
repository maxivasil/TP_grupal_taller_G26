#ifndef SERVER_CMD_REGISTER_H
#define SERVER_CMD_REGISTER_H

#include <unordered_map>
#include <vector>

#include "../../common/constants.h"

#include "cmd_base_server.h"

class ServerRegisteredCommands {
public:
    ServerRegisteredCommands();
    const std::unordered_map<uint8_t,
                             std::function<ClientToServerCmd_Server*(const std::vector<uint8_t>&,
                                                                     const uint32_t client_id)>>&
            get_recv_registry() const;

private:
    std::unordered_map<uint8_t, std::function<ClientToServerCmd_Server*(const std::vector<uint8_t>&,
                                                                        const uint32_t client_id)>>
            recv_registry;

    std::unordered_map<uint8_t, std::function<ClientToServerCmd_Server*(const std::vector<uint8_t>&,
                                                                        const uint32_t client_id)>>
            build_server_recv_command_registry();
};

#endif  // SERVER_CMD_REGISTER_H
