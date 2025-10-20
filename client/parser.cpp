#include "parser.h"

cmd ClientParser::parse_and_filter_line(int& lines_to_read) const {
    std::string cmd;
    if (!(std::cin >> cmd) || cmd == "exit") {
        return EXIT;
    } else if (cmd == "nitro") {
        return NITRO;
    } else if (cmd == "read") {
        return parse_cmd_read(lines_to_read);
    }
    return INVALID;
}

cmd ClientParser::parse_cmd_read(int& lines_to_read) const {
    std::string num_str;
    std::getline(std::cin, num_str);
    num_str.erase(0, num_str.find_first_not_of(" \t"));
    try {
        int n = std::stoi(num_str);
        if (n <= 0) {
            return INVALID;
        }
        lines_to_read = n;
        return READ;
    } catch (...) {
        return INVALID;
    }
}
