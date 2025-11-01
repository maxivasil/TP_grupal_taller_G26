#ifndef CLIENT_PARSER_H
#define CLIENT_PARSER_H

#include <iostream>
#include <string>

enum cmd { EXIT, MOVE, READ, INVALID };

struct ParsedCommand {
    cmd type;
    int direction = -1;
    int lines_to_read = -1;
};

class ClientParser {
public:
    /**
     * Parsea la línea ingresada por el usuario y devuelve el comando correspondiente.
     * Si el comando es READ, actualiza lines_to_read con la cantidad de líneas a leer.
     * @param lines_to_read Referencia a un entero que se actualizará si el comando es READ.
     * @return El comando parseado (EXIT, MOVE, READ, INVALID).
     */
    ParsedCommand parse_and_filter_line() const;

private:
    /**
     * Parsea la cantidad de lineas a leer en caso de que el comando sea READ.
     * @param lines_to_read Referencia a un entero que se actualizará con la cantidad de líneas a
     * leer.
     * @return El comando READ si el parseo es exitoso, INVALID en caso contrario.
     */
    ParsedCommand parse_cmd_read() const;

    /**
     * Parsea la dirección de movimiento si el comando es MOVE.
     */
    ParsedCommand parse_cmd_move() const;
};

#endif
