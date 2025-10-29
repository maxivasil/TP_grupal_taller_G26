#ifndef WORLD_H
#define WORLD_H

#include <SDL2/SDL.h>
#include <box2d/box2d.h>  // Add this line
#include <vector>

/**
 * @class World
 * @brief Contiene los objetos estáticos del mundo (edificios, calles, etc.).
 */
class World {
public:
    /**
     * @brief Crea un mundo con contenido básico (edificios generados aleatoriamente).
     */
    World();

    /**
     * @brief Renderiza todos los elementos del mundo visibles por la cámara.
     * @param renderer Renderer de SDL.
     * @param camera Cámara actual (vista del jugador).
     */
    void render(SDL_Renderer* renderer, const SDL_FRect& camera) const;

    SDL_FRect getBounds() const;

    const std::vector<SDL_FRect>& getBuildings() const;

    b2WorldId getWorldId() const;

private:
    std::vector<SDL_FRect> buildings; ///< Lista de edificios del mundo.
    b2WorldId worldId;
};

#endif // WORLD_H
