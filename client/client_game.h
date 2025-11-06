#ifndef GAME_H
#define GAME_H

#include <algorithm>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>
#include <SDL_image.h>

#include "camera.h"
#include "session.h"
#include "minimap.h"

struct RenderCar {
    SDL_Rect src;
    SDL_Rect dst;
    float angle;
};

class Game {
private:
    ClientSession& client_session;

    uint8_t client_id = 0;
    Camera camera;
    Minimap minimap;
    std::map<int, std::shared_ptr<SDL2pp::Texture>> textures;

    std::vector<CarSnapshot> snapshots;
    std::vector<RenderCar> carsToRender;
    
    // Para testing del minimap sin snapshots
    float testPlayerX = 350.0f;
    float testPlayerY = 300.0f;
    float testPlayerAngle = 0.0f;
    bool showMinimap = true;  // Toggle con M

    SDL_Rect src;
    SDL_Rect dst;

    bool handleEvents(SDL2pp::Renderer& renderer);

    void init_textures(SDL2pp::Renderer& renderer);

    bool update(SDL2pp::Renderer& renderer, ServerToClientSnapshot cmd_snapshot);

    void render(SDL2pp::Renderer& renderer);

public:
    explicit Game(ClientSession& client_session);

    int start();
    void update_snapshots(const std::vector<CarSnapshot>& snapshots);
};

#endif
