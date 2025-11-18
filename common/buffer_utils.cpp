#include "buffer_utils.h"

#include <cstring>

#include <arpa/inet.h>

void BufferUtils::append_bytes(std::vector<uint8_t>& buffer, const void* data, size_t size) {
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
    buffer.insert(buffer.end(), bytes, bytes + size);
}

void BufferUtils::append_uint8(std::vector<uint8_t>& buf, uint8_t v) {
    BufferUtils::append_bytes(buf, &v, sizeof(uint8_t));
}

void BufferUtils::append_uint16(std::vector<uint8_t>& buf, uint16_t v) {
    uint16_t net = htons(v);
    BufferUtils::append_bytes(buf, &net, sizeof(uint16_t));
}

void BufferUtils::append_uint32(std::vector<uint8_t>& buf, uint32_t v) {
    uint32_t net = htonl(v);
    BufferUtils::append_bytes(buf, &net, sizeof(uint32_t));
}

void BufferUtils::append_float(std::vector<uint8_t>& buf, float f) {
    uint32_t bits;
    memcpy(&bits, &f, sizeof(float));
    bits = htonl(bits);
    BufferUtils::append_bytes(buf, &bits, sizeof(uint32_t));
}
