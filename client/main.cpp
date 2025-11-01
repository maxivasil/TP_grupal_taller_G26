#include <exception>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#include "../common/queue.h"

#include "client_game.h"
#include "session.h"

int main(int argc, const char* argv[]) {
    try {
        int ret = 1;
        if (argc != 3) {
            std::cerr << "Bad program call. Expected " << argv[0] << " <hostname> <servicename>\n";
            return ret;
        }
        const char* hostname = argv[1];
        const char* servname = argv[2];
        Queue<ServerToClientCmd_Client*> recv_queue(UINT32_MAX);
        ClientSession session(hostname, servname, recv_queue);
        Game game(session);
        game.start();
        ret = session.run();
        return ret;
    } catch (const std::exception& err) {
        std::cerr << "Something went wrong and an exception was caught: " << err.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Something went wrong and an unknown exception was caught.\n";
        return 1;
    }
}
