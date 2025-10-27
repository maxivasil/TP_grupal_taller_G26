#include "protocol.h"

#include <cstring>
#include <sstream>
#include <string>
#include <utility>

#include <arpa/inet.h>  // htonl, ntohl

Protocol::Protocol(Socket&& socket): skt(std::move(socket)) {}
Protocol::Protocol(const char* hostname, const char* servname): skt(hostname, servname) {}

size_t Protocol::compute_expected_size(uint8_t header) const {
    uint16_t number_of_cars;
    switch (header) {
        case SEND_CARS_WITH_NITRO:
            return sizeof(header) + sizeof(number_of_cars) + sizeof(header);

        case ACTIVATE_NITRO:
            return sizeof(header);

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

int Protocol::recv_cars_with_nitro(Socket& skt, std::vector<uint8_t>& buffer) const {
    uint8_t temp[sizeof(uint16_t) + sizeof(uint8_t)];
    int ret = skt.recvall(temp, sizeof(temp));
    if (ret <= 0) {
        throw std::runtime_error("Error receiving Cars with Nitro");
    }
    uint16_t net_value;
    std::memcpy(&net_value, temp, sizeof(net_value));
    uint16_t cars_with_nitro = ntohs(net_value);
    uint8_t second_header = temp[2];
    BufferUtils::append_bytes(buffer, &cars_with_nitro, sizeof(cars_with_nitro));
    BufferUtils::append_bytes(buffer, &second_header, sizeof(second_header));
    return ret;
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
    if (header == ACTIVATE_NITRO) {
        return buffer;  // No payload
    }
    if (header == MOVE_COMMAND) {
        uint8_t direction;
        ret = skt.recvall(&direction, sizeof(direction));
        if (ret <= 0)
            throw std::runtime_error("Error receiving move command direction");
        BufferUtils::append_bytes(buffer, &direction, sizeof(direction));
        return buffer;
    }
    if (header == SEND_CARS_WITH_NITRO) {
        ret = recv_cars_with_nitro(skt, buffer);
        if (ret <= 0)
            throw std::runtime_error("Error receiving cars with nitro");
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

int Protocol::send_cars_with_nitro(uint16_t cars_with_nitro, uint8_t second_header) {
    std::vector<uint8_t> buffer;
    buffer.push_back(SEND_CARS_WITH_NITRO);
    uint16_t network_cars = htons(cars_with_nitro);
    BufferUtils::append_bytes(buffer, &network_cars, sizeof(network_cars));
    BufferUtils::append_bytes(buffer, &second_header, sizeof(second_header));
    return send_message(skt, buffer);
}

int Protocol::receive_activate_nitro() {
    auto msg = recv_full_message(skt);
    if (msg.size() < compute_expected_size(ACTIVATE_NITRO))
        return -1;
    int code = static_cast<int>(msg[0]);
    if (code == ACTIVATE_NITRO) {
        return code;
    }
    return -1;
}

int Protocol::send_activate_nitro() {
    std::vector<uint8_t> buffer;
    buffer.push_back(ACTIVATE_NITRO);
    return send_message(skt, buffer);
}

int Protocol::receive_cars_with_nitro() {
    auto msg = recv_full_message(skt);
    if (msg.size() < compute_expected_size(SEND_CARS_WITH_NITRO))
        return -1;
    int code = static_cast<int>(msg[3]);
    if (code == INFORM_NITRO_ACTIVATED || code == INFORM_NITRO_EXPIRED) {
        return code;
    }
    return -1;
}
