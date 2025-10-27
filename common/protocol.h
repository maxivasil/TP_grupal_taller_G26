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
     * @brief Calcula el tamaño esperado de un comando dado su header y el size consecuente.
     * @param header El header del comando.
     * @return El tamaño esperado del comando completo.
     */
    size_t compute_expected_size(uint8_t header) const;

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

    // Métodos auxiliares privados para manejar la recepción de mensajes.

    /**
     * Recibe el encabezado (header) de un mensaje desde el socket.
     * @param skt Socket desde el cual recibir el encabezado.
     * @param ret Referencia para almacenar el resultado de la operación de recepción.
     * @return El byte del encabezado recibido. Si la conexión se cierra, retorna 0.
     */
    uint8_t recv_header(Socket& skt, int& ret) const;
};

#endif
