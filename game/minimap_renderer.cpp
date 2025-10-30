#include "minimap_renderer.h"
#include <cmath>

MinimapRenderer::MinimapRenderer(int size)
    : size(size), renderTarget(nullptr) {}

void MinimapRenderer::render(SDL_Renderer* renderer, const World& world, const Car& car) {
    if (!renderTarget)
        renderTarget = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                         SDL_TEXTUREACCESS_TARGET, size, size);

    SDL_SetTextureBlendMode(renderTarget, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, renderTarget);

    // Fondo
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 200);
    SDL_RenderClear(renderer);

    // Escala mundo → minimapa
    SDL_FRect worldBounds = world.getBounds();
    float scale = (float)size / worldBounds.w;

    b2Vec2 carPos = car.getPosition();

    // Dibujar edificios (posición relativa al auto)
    SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
    for (const auto& building : world.getBuildings()) {
        float bx = (building.x - carPos.x) * scale + size / 2;
        float by = (building.y - carPos.y) * scale + size / 2;
        SDL_Rect miniRect = { 
            (int)bx, (int)by, 
            std::max(2, (int)(building.w * scale)), 
            std::max(2, (int)(building.h * scale)) 
        };
        SDL_RenderFillRect(renderer, &miniRect);
    }

    // Auto centrado (triángulo pequeño apuntando arriba)
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Point miniTriangle[4] = {
        {size / 2, size / 2 - 6},       // Punta
        {size / 2 - 4, size / 2 + 4},   // Izquierda
        {size / 2 + 4, size / 2 + 4},   // Derecha
        {size / 2, size / 2 - 6}        // Cierra el triángulo
    };
    SDL_RenderDrawLines(renderer, miniTriangle, 4);
    
    // Punto amarillo en el frente del auto en minimapa
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_Rect miniDot = {size / 2 - 2, size / 2 - 8, 4, 4};
    SDL_RenderFillRect(renderer, &miniDot);

    SDL_SetRenderTarget(renderer, nullptr);
    
    // Renderizar minimapa SIN rotación
    SDL_Rect dest = { 20, 20, size, size };
    SDL_RenderCopy(renderer, renderTarget, nullptr, &dest);  // Sin SDL_RenderCopyEx
}
