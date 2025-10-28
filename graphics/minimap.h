#pragma once
#include <SDL2/SDL.h>
#include "player.h"
#include "camera.h"

class Minimap {
public:
    Minimap(SDL_Renderer* renderer, int worldWidth, int worldHeight, int width, int height);
    ~Minimap();

    void renderToTexture(SDL_Renderer* renderer, const Player& player, const Camera& camera);
    void drawToScreen(SDL_Renderer* renderer, int x, int y, Uint8 alpha = 180);

private:
    int worldWidth, worldHeight;
    int width, height;
    float scale;
    SDL_Texture* renderTarget;
};
