#ifndef GAME_H
#define GAME_H

#include <algorithm>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>
#include <SDL_image.h>

#include "camera.h"
#include "checkpoint_arrow.h"
#include "collision_explosion.h"
#include "car_sound_engine.h"
#include "hud.h"
#include "minimap.h"
#include "session.h"
#include "cmd/server_to_client_raceResults.h"

struct RenderCar {
    SDL_Rect src;
    SDL_Rect dst;
    float angle;
    bool onBridge;
};

enum class GameState { PLAYING, WON, LOST };

class Game {
private:
    ClientSession& client_session;

    uint8_t client_id = 0;
    Camera camera;
    Minimap minimap;
    HUD hud;
    CheckpointArrow arrow;
    CollisionExplosion explosion;
    CarSoundEngine carSoundEngine;
    std::map<int, std::shared_ptr<SDL2pp::Texture>> textures;

    std::vector<CarSnapshot> snapshots;
    std::vector<RenderCar> carsToRender;

    // Para testing del minimap sin snapshots
    float testPlayerX = 350.0f;
    float testPlayerY = 300.0f;
    float testPlayerAngle = 0.0f;
    bool showMinimap = true;  // Toggle con M

    // HUD tracking
    int currentCheckpoint = 0;
    int totalCheckpoints = 4;
    float raceStartTime = 0.0f;
    
    // Checkpoints for the race
    std::vector<RaceCheckpoint> trackCheckpoints;

    // Collision tracking (to detect transition from no collision to collision)
    std::unordered_map<int, bool> previousCollisionState;

    // Health tracking (to detect when player takes damage)
    std::unordered_map<int, float> previousHealthState;

    // Speed calculation (client-side)
    float lastPlayerX = 0.0f;
    float lastPlayerY = 0.0f;
    Uint32 lastSpeedUpdateTime = 0;

    // Game state
    GameState gameState = GameState::PLAYING;
    std::string endGameMessage = "";
    Uint32 endGameTime = 0;
    
    // Race results
    std::vector<ClientPlayerResult> raceResults;
    bool hasRaceResults = false;

    SDL_Rect src;
    SDL_Rect dst;

    bool handleEvents(SDL2pp::Renderer& renderer);

    void init_textures(SDL2pp::Renderer& renderer);

    bool update(SDL2pp::Renderer& renderer, ServerToClientSnapshot cmd_snapshot);

    void render(SDL2pp::Renderer& renderer);

    void renderEndGameScreen(SDL2pp::Renderer& renderer);

    void setWon();

    void setLost();

public:
    explicit Game(ClientSession& client_session);

    int start();
    void update_snapshots(const std::vector<CarSnapshot>& snapshots);
    void setRaceResults(const std::vector<ClientPlayerResult>& results);

};

#endif
