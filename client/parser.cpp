#include "parser.h"

#include <iostream>

ParsedCommand ClientParser::parse_and_filter_line() const {
    std::string cmd;
    if (!(std::cin >> cmd) || cmd == "exit") {
        return {EXIT};
    } else if (cmd == "move") {
        return parse_cmd_move();
    } else if (cmd == "read") {
        return parse_cmd_read();
    }
    return {INVALID};
}

ParsedCommand ClientParser::parse_cmd_move() const {
    std::string dir_str;
    std::cin >> dir_str;

    int dir = -1;
    if (dir_str == "up")
        dir = 0;
    else if (dir_str == "down")
        dir = 1;
    else if (dir_str == "left")
        dir = 2;
    else if (dir_str == "right")
        dir = 3;

    if (dir == -1)
        return {INVALID};
    return {MOVE, dir};
}

ParsedCommand ClientParser::parse_cmd_read() const {
    std::string num_str;
    std::getline(std::cin, num_str);
    num_str.erase(0, num_str.find_first_not_of(" \t"));
    try {
        int n = std::stoi(num_str);
        if (n <= 0) {
            return {INVALID};
        }
        return {READ, 0, n};
    } catch (...) {
        return {INVALID};
    }
}
