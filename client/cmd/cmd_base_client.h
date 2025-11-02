#ifndef CMD_BASE_CLIENT_H
#define CMD_BASE_CLIENT_H

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "../parser.h"

class Game;

class ClientToServerCmd_Client {
public:
    virtual ~ClientToServerCmd_Client() = default;

    // Serializa el comando a bytes
    virtual std::vector<uint8_t> to_bytes() const = 0;
};

class ServerToClientCmd_Client {
public:
    virtual ~ServerToClientCmd_Client() = default;

    // Ejecuta el comando en el cliente (desde el servidor)
    virtual void execute(Game& game) = 0;  // ClientSession& client

    static ServerToClientCmd_Client* from_bytes(
            const std::vector<uint8_t>& data,
            const std::unordered_map<
                    uint8_t, std::function<ServerToClientCmd_Client*(const std::vector<uint8_t>&)>>&
                    registry);

    static constexpr uint8_t INVALID = 0xFF;
};

#endif  // CMD_BASE_CLIENT_H
