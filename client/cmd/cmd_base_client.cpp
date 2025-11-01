#include "cmd_base_client.h"

#include <stdexcept>

#include "../../common/constants.h"

#include "server_to_client_snapshot.h"

ServerToClientCmd_Client* ServerToClientCmd_Client::from_bytes(
        const std::vector<uint8_t>& data,
        const std::unordered_map<
                uint8_t, std::function<ServerToClientCmd_Client*(const std::vector<uint8_t>&)>>&
                registry) {

    if (data.empty())
        return nullptr;

    uint8_t header = data[0];
    auto it = registry.find(header);
    if (it == registry.end()) {
        throw std::runtime_error("Unknown command header: " + std::to_string(header));
    }

    return it->second(data);
}
