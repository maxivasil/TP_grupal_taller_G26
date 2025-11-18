#include "cmd_base_server.h"

#include <stdexcept>

#include "../../common/constants.h"

#include "client_to_server_move.h"

ClientToServerCmd_Server* ClientToServerCmd_Server::from_bytes(
        const std::vector<uint8_t>& data,
        const std::unordered_map<
                uint8_t, std::function<ClientToServerCmd_Server*(const std::vector<uint8_t>&,
                                                                 const int client_id)>>& registry,
        const int client_id) {

    if (data.empty())
        return nullptr;

    uint8_t header = data[0];
    auto it = registry.find(header);
    if (it != registry.end()) {
        return it->second(data, client_id);
    } else {
        throw std::runtime_error("ClientToServerCmd_Server::from_bytes: Unknown command header " +
                                 std::to_string(header));
    }
}
