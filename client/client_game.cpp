#include "client_game.h"

#include <unistd.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define FPS 1000 / 30

#define DATA_PATH "assets/"

Game::Game(ClientSession& client_session):
        client_session(client_session), camera(WINDOW_WIDTH, WINDOW_HEIGHT) {}

int Game::start() {
    try {
        SDL2pp::SDL sdl(SDL_INIT_VIDEO);

        std::string titulo = "NFS-2D";

        SDL2pp::Window window(titulo, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);

        SDL2pp::Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        init_textures(renderer);

        auto t1 = SDL_GetTicks();
        auto rate = FPS;

        while (true) {
            t1 = SDL_GetTicks();  // t1 guarda el tiempo actual en milisegundos

            if (handleEvents(renderer)) {
                return 0;
            }

            update(renderer);

            render(renderer);

            auto t2 = SDL_GetTicks();     // t2 guarda el tiempo actual en milisegundos
            int rest = rate - (t2 - t1);  // Tiempo que falta para la proxima iteracion.
                                          // Al tiempo buscado 'rate' se le resta
                                          // la diferencia entre 't2' y 't1', que es el tiempo que
                                          // se tardo en renderizar

            // Retrasado en comparacion al ritmo deseado
            if (rest < 0) {
                auto behind = -rest;                 // Tiempo de retraso
                auto lost = behind - behind % rate;  // Tiempo perdido
                t1 += lost;  // 't1' se adelanta en 'lost' milisegundos porque esos tiempos
                             // se perdieron a causa del retraso
            } else {         // A tiempo o adelantado
                SDL_Delay(rest);
            }

            t1 += rate;  // Aumentamos 't1' en 'rate' para la proxima iteracion
        }

        return 0;

    } catch (const std::exception& err) {
        std::cerr << "Something went wrong and an exception was caught: " << err.what() << "\n";
        return -1;
    }
}

bool Game::handleEvents(SDL2pp::Renderer& renderer) {

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {

            case SDL_QUIT:
                return true;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    camera.setDimensions(renderer.GetOutputWidth(), renderer.GetOutputHeight());
                }
        }

        if (event.type == SDL_KEYDOWN) {

            switch (event.key.keysym.sym) {

                case SDLK_ESCAPE:
                case SDLK_q:
                    return true;

                case SDLK_RIGHT:
                    carWorldX += speed;
                    break;

                case SDLK_LEFT:
                    carWorldX -= speed;
                    break;

                case SDLK_UP:
                    carWorldY -= speed;
                    break;

                case SDLK_DOWN:
                    carWorldY += speed;
                    break;
            }
        }
    }
    return false;
}

bool Game::update(SDL2pp::Renderer& renderer) {
    // Obtener tamaño de la textura original
    int texW = textures[0]->GetWidth();
    int texH = textures[0]->GetHeight();

    camera.follow(carWorldX, carWorldY);

    src = camera.getSrcRect(texW, texH);
    dst = {0, 0, renderer.GetOutputWidth(), renderer.GetOutputHeight()};

    float scale = float(dst.w) / float(src.w);

    // Sprite del primer auto verde
    int carW = textures[1]->GetWidth() / 12;
    int carH = textures[1]->GetHeight() / 16;

    // Posición relativa a cámara
    float relX = (carWorldX - src.x) * scale;
    float relY = (carWorldY - src.y) * scale;

    // Rectángulo destino (en pantalla)
    carDst = {int(relX - (carW * scale) / 2), int(relY - (carH * scale) / 2), int(carW * scale),
              int(carH * scale)};

    // Asumimos primer coche en sprite sheet
    carSrc = {0, 0, carW, carH};

    return false;  // Aca quizas haya que devolver true una vez que termine la partida?
}

void Game::render(SDL2pp::Renderer& renderer) {
    renderer.Clear();

    renderer.Copy(*textures[0], src, dst);
    renderer.Copy(*textures[1], carSrc, carDst);

    renderer.Present();
}

void Game::init_textures(SDL2pp::Renderer& renderer) {
    textures[0] = std::make_shared<SDL2pp::Texture>(
            renderer, SDL2pp::Surface(DATA_PATH "cities/Liberty_City.png").SetColorKey(true, 0));
    textures[1] = std::make_shared<SDL2pp::Texture>(
            renderer,
            SDL2pp::Surface(DATA_PATH "cars/Cars.png")
                    .SetColorKey(
                            true,
                            SDL_MapRGB(SDL2pp::Surface(DATA_PATH "cars/Cars.png").Get()->format,
                                       163, 163, 13)));
}
