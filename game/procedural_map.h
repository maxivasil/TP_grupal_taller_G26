#ifndef PROCEDURAL_MAP_H
#define PROCEDURAL_MAP_H

#include <vector>
#include <SDL2/SDL.h>
#include <box2d/box2d.h>

struct Building {
    float x, y, width, height;
};

class ProceduralMap {
private:
    std::vector<Building> buildings;
    int mapWidth, mapHeight;
    
public:
    ProceduralMap(int width = 5000, int height = 5000, int seed = 42);
    
    void render(SDL_Renderer* renderer, const SDL_FRect& camera) const;
    void addToPhysicsWorld(b2WorldId worldId);
    const std::vector<Building>& getBuildings() const { return buildings; }
};

#endif