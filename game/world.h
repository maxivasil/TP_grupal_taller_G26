#ifndef WORLD_H
#define WORLD_H

#include <SDL2/SDL.h>
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

    /// Devuelve el tamaño total del mundo.
    SDL_FRect getBounds() const;

    const std::vector<SDL_FRect>& World::getBuildings() const;

private:
    std::vector<SDL_FRect> buildings; ///< Lista de edificios del mundo.
};

#endif // WORLD_H
