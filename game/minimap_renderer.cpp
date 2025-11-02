#include "minimap_renderer.h"
#include <cmath>
#include <algorithm>

MinimapRenderer::MinimapRenderer(int size)
    : size(size), renderTarget(nullptr) {}

MinimapRenderer::~MinimapRenderer() {
    if (renderTarget) SDL_DestroyTexture(renderTarget);
}

void MinimapRenderer::render(SDL_Renderer* renderer, const World& world, const Car& car, const City& city) {
    if (!renderTarget)
        renderTarget = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                         SDL_TEXTUREACCESS_TARGET, size, size);

    SDL_SetTextureBlendMode(renderTarget, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, renderTarget);

    // Fondo gris oscuro (las calles)
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderClear(renderer);

    b2Vec2 carPos = car.getPosition();
    
    // Escala: mostrar 2000x2000 unidades de mundo en el minimapa
    float worldSize = 2000.0f;
    float scale = (float)size / worldSize;

    // ✅ Renderizar edificios del YAML con colores reales
    for (const auto& obj : city.getStaticObjects()) {
        float bx = (obj.x - carPos.x) * scale + size / 2;
        float by = (obj.y - carPos.y) * scale + size / 2;
        
        // Solo renderizar si está dentro del minimapa
        if (bx + 2 < 0 || bx > size || by + 2 < 0 || by > size) {
            continue;
        }
        
        // ✅ Colores según el tipo
        if (obj.isUp) {
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);  // Gris oscuro (nivel superior)
        } else {
            SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);  // Gris claro (nivel inferior)
        }
        
        SDL_Rect miniRect = {
            (int)bx, (int)by,
            std::max(1, (int)(obj.width * scale)),
            std::max(1, (int)(obj.height * scale))
        };
        SDL_RenderFillRect(renderer, &miniRect);
    }
    
    // ✅ Renderizar puentes en amarillo
    for (const auto& sensor : city.getBridgeSensors()) {
        float bx = (sensor.x - carPos.x) * scale + size / 2;
        float by = (sensor.y - carPos.y) * scale + size / 2;
        
        if (bx + 2 < 0 || bx > size || by + 2 < 0 || by > size) {
            continue;
        }
        
        SDL_SetRenderDrawColor(renderer, 200, 200, 0, 200);  // Amarillo semitransparente
        
        SDL_Rect miniRect = {
            (int)bx, (int)by,
            std::max(1, (int)(sensor.width * scale)),
            std::max(1, (int)(sensor.height * scale))
        };
        SDL_RenderFillRect(renderer, &miniRect);
    }

    // Auto centrado con rotación
    float carAngle = car.getAngle();
    float cx = size / 2.0f;
    float cy = size / 2.0f;
    
    float localPoints[3][2] = {
        {0.0f, -5.0f},    // Punta delantera
        {-4.0f, 4.0f},    // Trasera izquierda
        {4.0f, 4.0f}      // Trasera derecha
    };
    
    SDL_Point miniTriangle[4];
    float cosA = std::cos(carAngle);
    float sinA = std::sin(carAngle);
    
    for (int i = 0; i < 3; ++i) {
        float rotX = localPoints[i][0] * cosA - localPoints[i][1] * sinA;
        float rotY = localPoints[i][0] * sinA + localPoints[i][1] * cosA;
        
        miniTriangle[i].x = (int)(cx + rotX);
        miniTriangle[i].y = (int)(cy + rotY);
    }
    miniTriangle[3] = miniTriangle[0];
    
    // ✅ Triángulo blanco del auto (más visible)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLines(renderer, miniTriangle, 4);
    
    // ✅ Punto rojo en el frente
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect miniDot = {
        miniTriangle[0].x - 2, 
        miniTriangle[0].y - 2, 
        4, 4
    };
    SDL_RenderFillRect(renderer, &miniDot);

    SDL_SetRenderTarget(renderer, nullptr);
    
    SDL_Rect dest = { 10, 10, size, size };
    SDL_RenderCopy(renderer, renderTarget, nullptr, &dest);
}
