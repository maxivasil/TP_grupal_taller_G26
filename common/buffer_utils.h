#ifndef BUFFER_UTILS_H
#define BUFFER_UTILS_H

#include <cstdint>
#include <vector>

class BufferUtils {
public:
    /**
     * Agrega bytes a un buffer al final.
     * @param buffer El buffer al que se agregarán los bytes.
     * @param data Puntero a los datos que se agregarán.
     * @param size Tamaño de los datos en bytes.
     */
    static void append_bytes(std::vector<uint8_t>& buffer, const void* data, size_t size);
    static void append_uint8(std::vector<uint8_t>& buf, uint8_t v);
    static void append_uint16(std::vector<uint8_t>& buf, uint16_t v);
    static void append_uint32(std::vector<uint8_t>& buf, uint32_t v);
    static void append_float(std::vector<uint8_t>& buf, float f);
    static void read_uint8(const std::vector<uint8_t>& buf, size_t& offset, uint8_t& v);
    static void read_uint16(const std::vector<uint8_t>& buf, size_t& offset, uint16_t& v);
    static void read_uint32(const std::vector<uint8_t>& buf, size_t& offset, uint32_t& v);
    static void read_float(const std::vector<uint8_t>& buf, size_t& offset, float& f);
};

#endif
