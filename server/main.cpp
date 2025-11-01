#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>

#include "session.h"

int main(int argc, char* argv[]) {
    try {
        int ret = 1;
        const char* servname = NULL;
        if (argc != 2) {
            std::cerr << "Bad program call. Expected " << argv[0] << " <servicename>\n";
            return ret;
        }
        servname = argv[1];
        ServerSession session(servname);
        session.run();
        ret = 0;
        return ret;
    } catch (const std::exception& err) {
        std::cerr << "Something went wrong and an exception was caught: " << err.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Something went wrong and an unknown exception was caught.\n";
        return 1;
    }
}
