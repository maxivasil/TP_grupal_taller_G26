#include "protocol.h"

#include <cstring>
#include <sstream>
#include <string>
#include <utility>

#include <arpa/inet.h>  // htonl, ntohl

Protocol::Protocol(Socket&& socket): skt(std::move(socket)) {}
Protocol::Protocol(const char* hostname, const char* servname): skt(hostname, servname) {}

size_t Protocol::compute_expected_size(uint8_t header) const {
    switch (header) {
        case MOVE_COMMAND:
            return sizeof(header) + sizeof(uint8_t);  // direction

        default:
            throw std::runtime_error("Unknown header");
    }
}

uint8_t Protocol::recv_header(Socket& skt, int& ret) const {
    uint8_t header;
    ret = skt.recvall(&header, 1);
    if (ret == 0)
        return {};  // Connection closed
    if (ret < 0)
        throw std::runtime_error("Error receiving header");
    return header;
}

std::vector<uint8_t> Protocol::recv_full_message(Socket& skt) {
    int ret;
    uint8_t header = recv_header(skt, ret);
    if (ret == 0)
        return {};  // connection closed
    std::vector<uint8_t> buffer{header};
    if (header == 0) {
        return {};
    }
    if (header == MOVE_COMMAND) {
        uint8_t direction;
        ret = skt.recvall(&direction, sizeof(direction));
        if (ret <= 0)
            throw std::runtime_error("Error receiving move command direction");
        BufferUtils::append_bytes(buffer, &direction, sizeof(direction));
        return buffer;
    } else {
        throw std::runtime_error("Unknown header received");
    }
    return buffer;
}

std::vector<uint8_t> Protocol::recv_full_message() { return recv_full_message(skt); }

int Protocol::send_message(Socket& skt, const std::vector<uint8_t>& msg) const {
    int bytes_sent = skt.sendall(msg.data(), msg.size());
    if (bytes_sent <= 0)
        throw std::runtime_error("Error sending message");
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
