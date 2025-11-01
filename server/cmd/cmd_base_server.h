#ifndef CMD_BASE_SERVER_H
#define CMD_BASE_SERVER_H

#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

struct ServerContext {
    class Race* race;
};

// Aquí van las definiciones de comandos del cliente al servidor
class ClientToServerCmd_Server {
protected:
    int client_id;

public:
    explicit ClientToServerCmd_Server(int client_id): client_id(client_id) {}
    virtual ~ClientToServerCmd_Server() = default;

    // Ejecuta el comando en el contexto del servidor (desde el cliente)
    virtual void execute(ServerContext& ctx) = 0;  // ServerSession& server, ClientSession& client

    // Fábrica estática para crear comandos desde bytes
    static ClientToServerCmd_Server* from_bytes(
            const std::vector<uint8_t>& data,
            const std::unordered_map<
                    uint8_t, std::function<ClientToServerCmd_Server*(
                                     const std::vector<uint8_t>&, const int client_id)>>& registry,
            const int client_id);
};

class ServerToClientCmd_Server {
public:
    virtual ~ServerToClientCmd_Server() = default;

    virtual std::vector<uint8_t> to_bytes() const = 0;

    virtual ServerToClientCmd_Server* clone() const = 0;
};

#endif  // CMD_BASE_SERVER_H
