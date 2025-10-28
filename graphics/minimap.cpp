#include "minimap.h"
#include <cmath>

Minimap::Minimap(SDL_Renderer* renderer, int worldWidth, int worldHeight, int width, int height)
    : worldWidth(worldWidth), worldHeight(worldHeight), width(width), height(height)
{
    scale = (float)width / worldWidth;

    // Crear textura para render target (offscreen)
    renderTarget = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        width,
        height
    );
    SDL_SetTextureBlendMode(renderTarget, SDL_BLENDMODE_BLEND);
}

Minimap::~Minimap() {
    if (renderTarget) SDL_DestroyTexture(renderTarget);
}

void Minimap::renderToTexture(SDL_Renderer* renderer, const Player& player, const Camera& camera) {
    // Guardar target previo
    SDL_Texture* prevTarget = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, renderTarget);

    // Limpiar con fondo semitransparente
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 20, 20, 30, 200);
    SDL_RenderClear(renderer);

    // Dibujar cuadrícula simple
    SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
    int step = 200;
    for (int i = 0; i < worldWidth; i += step) {
        int x = (int)(i * scale);
        SDL_RenderDrawLine(renderer, x, 0, x, height);
    }
    for (int j = 0; j < worldHeight; j += step) {
        int y = (int)(j * scale);
        SDL_RenderDrawLine(renderer, 0, y, width, y);
    }

    // Rectángulo de cámara
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect camRect = {
        (int)((camera.x - camera.width / 2) * scale),
        (int)((camera.y - camera.height / 2) * scale),
        (int)(camera.width * scale),
        (int)(camera.height * scale)
    };
    SDL_RenderDrawRect(renderer, &camRect);

    // Jugador
    float px = player.x * scale;
    float py = player.y * scale;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect pRect = { (int)(px - 3), (int)(py - 3), 6, 6 };
    SDL_RenderFillRect(renderer, &pRect);

    // Restaurar render target
    SDL_SetRenderTarget(renderer, prevTarget);
}

void Minimap::drawToScreen(SDL_Renderer* renderer, int x, int y, Uint8 alpha) {
    SDL_Rect dest = { x, y, width, height };
    SDL_SetTextureAlphaMod(renderTarget, alpha);
    SDL_RenderCopy(renderer, renderTarget, nullptr, &dest);
}
