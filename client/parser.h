#ifndef CLIENT_PARSER_H
#define CLIENT_PARSER_H

#include <iostream>
#include <string>

enum cmd { EXIT, NITRO, READ, INVALID };

class ClientParser {
public:
    /**
     * Parsea la línea ingresada por el usuario y devuelve el comando correspondiente.
     * Si el comando es READ, actualiza lines_to_read con la cantidad de líneas a leer.
     * @param lines_to_read Referencia a un entero que se actualizará si el comando es READ.
     * @return El comando parseado (EXIT, NITRO, READ, INVALID).
     */
    cmd parse_and_filter_line(int& lines_to_read) const;

private:
    /**
     * Parsea la cantidad de lineas a leer en caso de que el comando sea READ.
     * @param lines_to_read Referencia a un entero que se actualizará con la cantidad de líneas a
     * leer.
     * @return El comando READ si el parseo es exitoso, INVALID en caso contrario.
     */
    cmd parse_cmd_read(int& lines_to_read) const;
};

#endif
