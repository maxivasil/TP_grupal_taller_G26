#ifndef CLIENT_TO_SERVER_TOUR_SERVER_H
#define CLIENT_TO_SERVER_TOUR_SERVER_H

#include <string>
#include <vector>

#include "../common/constants.h"

#include "cmd_base_server.h"

class ClientToServerTour_Server: public ClientToServerCmd_Server {
public:
    ClientToServerTour_Server(std::string tourFile, uint32_t client_id);

    ~ClientToServerTour_Server() override = default;

    void execute(ServerContext& ctx) override;

    static ClientToServerTour_Server* from_bytes(const std::vector<uint8_t>& data,
                                                 const uint32_t client_id);

    const std::string& get_tour_file() const;

private:
    std::string tourFile;
};

#endif
