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

#include "audio/car_sound_engine.h"
#include "cmd/server_to_client_raceResults.h"
#include "graphics/camera.h"
#include "graphics/checkpoint_arrow.h"
#include "graphics/collision_explosion.h"
#include "graphics/hud.h"
#include "graphics/minimap.h"

#include "session.h"

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
    uint8_t client_id = UINT8_MAX;
    Camera camera;
    Minimap minimap;
    HUD hud;
    CheckpointArrow arrow;
    CollisionExplosion explosion;
    CarSoundEngine carSoundEngine;
    std::map<int, std::shared_ptr<SDL2pp::Texture>> textures;

    std::vector<CarSnapshot> snapshots;
    std::vector<RenderCar> carsToRender;

    float testPlayerX = 350.0f;
    float testPlayerY = 300.0f;
    float testPlayerAngle = 0.0f;
    bool showMinimap = true;

    int currentCheckpoint = 0;
    int totalCheckpoints = 4;
    float raceStartTime = 0.0f;

    std::vector<RaceCheckpoint> trackCheckpoints;

    std::unordered_map<int, bool> previousCollisionState;

    std::unordered_map<int, float> previousHealthState;

    float lastPlayerX = 0.0f;
    float lastPlayerY = 0.0f;
    Uint32 lastSpeedUpdateTime = 0;

    std::unordered_map<int, float> otherPlayersLastHealth;
    std::unordered_map<int, float> otherPlayersLastSpeed;

    Uint32 collisionFlashStartTime = 0;
    const Uint32 FLASH_DURATION_MS = 300;
    float lastCollisionIntensity = 0.0f;

    bool playerDestroyed = false;
    Uint32 destructionStartTime = 0;

    GameState gameState = GameState::PLAYING;
    std::string endGameMessage = "";
    Uint32 endGameTime = 0;

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
    float getScale(int w, int h) const;

public:
    explicit Game(ClientSession& client_session);

    int start();
    void setClientId(uint8_t id);
    void update_snapshots(const std::vector<CarSnapshot>& snapshots);
    void setRaceResults(const std::vector<ClientPlayerResult>& results);
};

#endif
