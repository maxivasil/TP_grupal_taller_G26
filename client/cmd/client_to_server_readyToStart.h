#ifndef CLIENT_TO_SERVER_READY_TO_START_H
#define CLIENT_TO_SERVER_READY_TO_START_H

#include <vector>
#include <string>

#include "../../common/buffer_utils.h"
#include "../../common/constants.h"

#include "cmd_base_client.h"

class ClientToServerReady: public ClientToServerCmd_Client {
public:
    ClientToServerReady(std::string car);
    std::vector<uint8_t> to_bytes() const override;  // Implementación de serialización
private:
    std::string car;
};
#endif  // CLIENT_TO_SERVER_READY_TO_START_H
