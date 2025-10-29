#include "world.h"
#include <cstdlib>

World::World() {
    // Crear el mundo de Box2D
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = {0.0f, 0.0f};  // Sin gravedad (vista top-down)
    worldId = b2CreateWorld(&worldDef);
    
    // Generar edificios
    for (int i = 0; i < 50; ++i) {
        SDL_FRect b = { (float)(rand() % 2000), (float)(rand() % 2000), 80, 80 };
        buildings.push_back(b);
    }
}

void World::render(SDL_Renderer* renderer, const SDL_FRect& camera) const {
    SDL_SetRenderDrawColor(renderer, 80, 100, 255, 255);
    for (const auto& b : buildings) {
        SDL_FRect screenRect = { b.x - camera.x, b.y - camera.y, b.w, b.h };
        SDL_RenderFillRectF(renderer, &screenRect);
    }
}

SDL_FRect World::getBounds() const {
    return { 0, 0, 2000, 2000 };
}

const std::vector<SDL_FRect>& World::getBuildings() const {
    return buildings;
}

b2WorldId World::getWorldId() const {
    return worldId;
}


