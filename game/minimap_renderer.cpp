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

    // Auto centrado con rotación
    float carAngle = car.getAngle();  // Ángulo en radianes de Box2D
    float cx = size / 2.0f;
    float cy = size / 2.0f;
    
    // Triángulo en coordenadas locales (apuntando hacia arriba en Box2D: Y negativo)
    float localPoints[3][2] = {
        {0.0f, -6.0f},    // Punta delantera
        {-4.0f, 4.0f},    // Trasera izquierda
        {4.0f, 4.0f}      // Trasera derecha
    };
    
    // Rotar cada punto según el ángulo del auto
    SDL_Point miniTriangle[4];
    float cosA = std::cos(carAngle);
    float sinA = std::sin(carAngle);
    
    for (int i = 0; i < 3; ++i) {
        // Rotar punto local
        float rotX = localPoints[i][0] * cosA - localPoints[i][1] * sinA;
        float rotY = localPoints[i][0] * sinA + localPoints[i][1] * cosA;
        
        // Trasladar al centro del minimapa
        miniTriangle[i].x = (int)(cx + rotX);
        miniTriangle[i].y = (int)(cy + rotY);
    }
    miniTriangle[3] = miniTriangle[0];  // Cerrar el triángulo
    
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawLines(renderer, miniTriangle, 4);
    
    // Punto amarillo en el frente (ya rotado)
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_Rect miniDot = {
        miniTriangle[0].x - 2, 
        miniTriangle[0].y - 2, 
        4, 4
    };
    SDL_RenderFillRect(renderer, &miniDot);

    SDL_SetRenderTarget(renderer, nullptr);
    
    // Renderizar minimapa
    SDL_Rect dest = { 20, 20, size, size };
    SDL_RenderCopy(renderer, renderTarget, nullptr, &dest);
}
