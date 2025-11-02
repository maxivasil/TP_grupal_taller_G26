#include "procedural_map.h"
#include <random>
#include <cmath>

ProceduralMap::ProceduralMap(int width, int height, int seed)
    : mapWidth(width), mapHeight(height) {
    
    std::mt19937 gen(seed);
    std::uniform_real_distribution<> dis(0, 1.0);
    
    // Grid de manzanas
    int blockSize = 200;
    int streetWidth = 40;
    
    for (int bx = 0; bx < mapWidth; bx += blockSize) {
        for (int by = 0; by < mapHeight; by += blockSize) {
            // Generar 2-4 edificios por manzana
            int numBuildings = 2 + static_cast<int>(dis(gen) * 3);
            
            for (int i = 0; i < numBuildings; ++i) {
                float buildingWidth = 30 + dis(gen) * 80;
                float buildingHeight = 40 + dis(gen) * 100;
                float buildingX = bx + streetWidth + dis(gen) * (blockSize - streetWidth - buildingWidth);
                float buildingY = by + streetWidth + dis(gen) * (blockSize - streetWidth - buildingHeight);
                
                buildings.push_back({buildingX, buildingY, buildingWidth, buildingHeight});
            }
        }
    }
}

void ProceduralMap::render(SDL_Renderer* renderer, const SDL_FRect& camera) const {
    // Renderizar calles (gris oscuro)
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_FRect streetRect = {-camera.x, -camera.y, (float)mapWidth, (float)mapHeight};
    SDL_RenderFillRectF(renderer, &streetRect);
    
    // Renderizar edificios (diferentes colores)
    for (const auto& building : buildings) {
        SDL_FRect rect = {
            building.x - camera.x,
            building.y - camera.y,
            building.width,
            building.height
        };
        
        // Solo renderizar si está visible
        if (rect.x + rect.w < 0 || rect.x > 800 ||
            rect.y + rect.h < 0 || rect.y > 600) {
            continue;
        }
        
        // Colores variados para los edificios
        int colorVariation = static_cast<int>(building.x + building.y) % 5;
        switch (colorVariation) {
            case 0: SDL_SetRenderDrawColor(renderer, 180, 100, 100, 255); break; // Rojo
            case 1: SDL_SetRenderDrawColor(renderer, 100, 180, 100, 255); break; // Verde
            case 2: SDL_SetRenderDrawColor(renderer, 100, 100, 180, 255); break; // Azul
            case 3: SDL_SetRenderDrawColor(renderer, 180, 180, 100, 255); break; // Amarillo
            case 4: SDL_SetRenderDrawColor(renderer, 180, 100, 180, 255); break; // Púrpura
        }
        
        SDL_RenderFillRectF(renderer, &rect);
        
        // Borde oscuro
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderDrawRectF(renderer, &rect);
    }
}

void ProceduralMap::addToPhysicsWorld(b2WorldId worldId) {
    for (const auto& building : buildings) {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_staticBody;
        bodyDef.position = {building.x + building.width / 2, building.y + building.height / 2};
        
        b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
        
        b2Polygon box = b2MakeBox(building.width / 2, building.height / 2);
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        b2CreatePolygonShape(bodyId, &shapeDef, &box);
    }
}