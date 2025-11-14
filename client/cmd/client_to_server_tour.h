#ifndef CLIENT_TO_SERVER_TOUR_CLIENT_H
#define CLIENT_TO_SERVER_TOUR_CLIENT_H

#include <cstdint>
#include <string>
#include <vector>

#include "../../common/buffer_utils.h"
#include "../../common/constants.h"

#include "cmd_base_client.h"

class ClientToServerTour: public ClientToServerCmd_Client {
public:
    explicit ClientToServerTour(std::string& tourFile);

    ~ClientToServerTour() override = default;

    std::vector<uint8_t> to_bytes() const override;

private:
    std::string tourFile;
};

#endif
