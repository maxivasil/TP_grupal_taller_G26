#ifndef CLIENT_TO_SERVER_READY_TO_START_CLIENT_H
#define CLIENT_TO_SERVER_READY_TO_START_CLIENT_H

#include <string>
#include <vector>

#include "../../common/buffer_utils.h"
#include "../../common/constants.h"

#include "cmd_base_client.h"

class ClientToServerReady_Client: public ClientToServerCmd_Client {
public:
    explicit ClientToServerReady_Client(std::string car);
    std::vector<uint8_t> to_bytes() const override;  // Implementación de serialización
private:
    std::string car;
};
#endif  // CLIENT_TO_SERVER_READY_TO_START_CLIENT_H
