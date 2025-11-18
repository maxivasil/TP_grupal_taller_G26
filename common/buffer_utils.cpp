#include "buffer_utils.h"

void BufferUtils::append_bytes(std::vector<uint8_t>& buffer, const void* data, size_t size) {
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
    buffer.insert(buffer.end(), bytes, bytes + size);
}
