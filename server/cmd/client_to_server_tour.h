#ifndef CLIENT_TO_SERVER_TOUR_H
#define CLIENT_TO_SERVER_TOUR_H

#include <string>
#include <vector>

#include "../common/constants.h"

#include "cmd_base_server.h"

class ClientToServerTour: public ClientToServerCmd_Server {
public:
    explicit ClientToServerTour(std::string& tourFile);

    ~ClientToServerTour() override = default;

    void execute(ServerContext& ctx) override;

    static ClientToServerTour* from_bytes(const std::vector<uint8_t>& data, const int client_id);

private:
    std::string tourFile;
};

#endif
