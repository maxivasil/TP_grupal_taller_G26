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

    // Dibujar edificios (necesitás un getter público para buildings)
    SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
    for (const auto& building : world.getBuildings()) {
        float bx = (building.x - carPos.x) * scale + size / 2;
        float by = (building.y - carPos.y) * scale + size / 2;
        SDL_Rect miniRect = { (int)bx, (int)by, 
                              std::max(2, (int)(building.w * scale)), 
                              std::max(2, (int)(building.h * scale)) };
        SDL_RenderFillRect(renderer, &miniRect);
    }

    // Auto centrado
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect carRect = { size / 2 - 3, size / 2 - 3, 6, 6 };
    SDL_RenderFillRect(renderer, &carRect);

    SDL_SetRenderTarget(renderer, nullptr);
    SDL_Rect dest = { 20, 20, size, size };
    SDL_RenderCopyEx(renderer, renderTarget, nullptr, &dest, 
                     -car.getAngle() * 180.0 / M_PI, nullptr, SDL_FLIP_NONE);
}
