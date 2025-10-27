#include <exception>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#include "../common/queue.h"

#include "session.h"

int main(int argc, const char* argv[]) {
    try {
        // // Initialize SDL library
        // SDL2pp::SDL sdl(SDL_INIT_VIDEO);

        // // Create main window: 640x480 dimensions, resizable, "SDL2pp demo" title
        // SDL2pp::Window window("SDL2pp demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        // 640, 480,
        //                       SDL_WINDOW_RESIZABLE);

        // // Create accelerated video renderer with default driver
        // SDL2pp::Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);

        // // Clear screen
        // renderer.Clear();

        // // Show rendered frame
        // renderer.Present();

        // // 5 second delay
        // SDL_Delay(5000);


        int ret = 1;
        if (argc != 3) {
            std::cerr << "Bad program call. Expected " << argv[0] << " <hostname> <servicename>\n";
            return ret;
        }
        const char* hostname = argv[1];
        const char* servname = argv[2];
        Queue<ServerToClientCmd_Client*> recv_queue(UINT32_MAX);
        ClientSession session(hostname, servname, recv_queue);
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
