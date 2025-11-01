#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstdint>
#include <string>
#include <vector>

#include "buffer_utils.h"
#include "constants.h"
#include "socket.h"

#define SHUTDOWN 2

class Protocol {
public:
    explicit Protocol(Socket&& socket);
    explicit Protocol(const char* hostname, const char* servname);
    ~Protocol() = default;  // dtor por defecto

    Protocol(const Protocol&) = delete;  // no copiar
    Protocol& operator=(const Protocol&) = delete;

    Protocol(Protocol&&) = default;  // permitir mover
    Protocol& operator=(Protocol&&) = default;

    /**
     * Recibe un mensaje completo desde el socket `skt` siguiendo el protocolo
     * definido en `protocol`.
     * @param skt Socket desde el cual recibir el mensaje.
     * @param protocol Protocolo a seguir para interpretar el mensaje.
     * @return Un vector de bytes que contiene el mensaje completo. En caso de error, lanza una
     * excepción.
     */
    std::vector<uint8_t> recv_full_message(Socket& skt);

    std::vector<uint8_t> recv_full_message();

    /**
     * Envía un mensaje completo a través del socket `skt`.
     * @param skt Socket a través del cual enviar el mensaje.
     * @param msg Vector de bytes que contiene el mensaje a enviar.
     * @return Número de bytes enviados. En caso de error, lanza una excepción.
     */
    int send_message(Socket& skt, const std::vector<uint8_t>& msg) const;

    int send_message(const std::vector<uint8_t>& msg);

    bool is_connection_closed() const;
    void close_connection();

    // FUNCIONES DE SERVIDOR

    //################ Métodos públicos para construir mensajes ################//

    // FUNCIONES DE CLIENTE

    //################ Métodos públicos para enviar mensajes al server ################//


private:
    Socket skt;
};

#endif
