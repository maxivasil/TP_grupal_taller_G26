#ifndef CLIENT_TO_SERVER_TOUR_CLIENT_H
#define CLIENT_TO_SERVER_TOUR_CLIENT_H

#include <cstdint>
#include <string>
#include <vector>

#include "../../common/buffer_utils.h"
#include "../../common/constants.h"

#include "cmd_base_client.h"

class ClientToServerTour_Client: public ClientToServerCmd_Client {
public:
    explicit ClientToServerTour_Client(std::string& tourFile);

    ~ClientToServerTour_Client() override = default;

    std::vector<uint8_t> to_bytes() const override;

private:
    std::string tourFile;
};

#endif
