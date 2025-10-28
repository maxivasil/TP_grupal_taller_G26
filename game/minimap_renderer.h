#ifndef MINIMAP_RENDERER_H
#define MINIMAP_RENDERER_H

#include <SDL2/SDL.h>
#include "car.h"
#include "world.h"

/**
 * @class MinimapRenderer
 * @brief Dibuja un minimapa rotante del mundo, centrado en el auto.
 */
class MinimapRenderer {
public:
    /**
     * @brief Construye un MinimapRenderer con un tamaño fijo.
     * @param size Tamaño en píxeles del minimapa cuadrado.
     */
    explicit MinimapRenderer(int size);

    /**
     * @brief Renderiza el minimapa sobre el renderer principal.
     * @param renderer Renderer de SDL.
     * @param world Referencia al mundo actual.
     * @param car Referencia al auto del jugador.
     */
    void render(SDL_Renderer* renderer, const World& world, const Car& car);

private:
    int size; ///< Tamaño (en píxeles) del minimapa cuadrado.
    SDL_Texture* renderTarget; ///< Textura temporal donde se dibuja el minimapa.
};

#endif // MINIMAP_RENDERER_H
