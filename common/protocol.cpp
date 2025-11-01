#include "protocol.h"

#include <cstring>
#include <sstream>
#include <string>
#include <utility>

#include <arpa/inet.h>  // htonl, ntohl

#include "../server/cmd/server_to_client_snapshot.h"

Protocol::Protocol(Socket&& socket): skt(std::move(socket)) {}
Protocol::Protocol(const char* hostname, const char* servname): skt(hostname, servname) {}

std::vector<uint8_t> Protocol::recv_full_message(Socket& skt) {
    uint32_t msg_size_network;
    int ret = skt.recvall(&msg_size_network, sizeof(msg_size_network));
    if (ret <= 0) {
        return {};  // conexión cerrada
    }

    uint32_t msg_size = ntohl(msg_size_network);
    if (msg_size == 0) {
        return {};
    }

    std::vector<uint8_t> buffer(msg_size);
    ret = skt.recvall(buffer.data(), msg_size);
    if (ret <= 0)
        throw std::runtime_error("Error receiving full message");

    return buffer;
}

std::vector<uint8_t> Protocol::recv_full_message() { return recv_full_message(skt); }

int Protocol::send_message(Socket& skt, const std::vector<uint8_t>& msg) const {
    uint32_t msg_size = htonl(static_cast<uint32_t>(msg.size()));
    skt.sendall(&msg_size, sizeof(msg_size));
    int bytes_sent = skt.sendall(msg.data(), msg.size());
    if (bytes_sent <= 0)
        return 0;
    // throw std::runtime_error("Error sending message");
    return bytes_sent;
}

int Protocol::send_message(const std::vector<uint8_t>& msg) { return send_message(skt, msg); }

void Protocol::close_connection() {
    skt.shutdown(SHUT_RDWR);
    skt.close();
}

// FUNCIONES DE SERVIDOR

bool Protocol::is_connection_closed() const {
    return skt.is_stream_recv_closed() || skt.is_stream_send_closed();
}
