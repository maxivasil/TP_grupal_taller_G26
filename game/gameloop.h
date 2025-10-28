#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include <SDL2/SDL.h>
#include "world.h"
#include "car.h"
#include "minimap_renderer.h"

/**
 * @class GameLoop
 * @brief Controla el flujo principal del juego: eventos, actualización y renderizado.
 */
class GameLoop {
public:
    GameLoop();
    ~GameLoop();

    /// Ejecuta el loop principal del juego.
    void run();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;

    World world;
    Car car;
    MinimapRenderer minimap;

    void handleEvents();
    void update(float deltaTime);
    void render();
};

#endif // GAME_LOOP_H
