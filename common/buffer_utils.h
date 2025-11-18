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
};

#endif
