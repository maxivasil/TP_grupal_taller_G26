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

#include "../server/game_logic/CarUpgrades.h"
#include "audio/car_sound_engine.h"
#include "cmd/server_to_client_raceResults_client.h"
#include "graphics/camera.h"
#include "graphics/checkpoint_arrow.h"
#include "graphics/collision_explosion.h"
#include "graphics/car_fire_effect.h"
#include "graphics/hud.h"
#include "graphics/minimap.h"

#include "session.h"

struct RenderCar {
    SDL_Rect src;
    SDL_Rect dst;
    float angle;
    bool onBridge;
    uint8_t car_id;
    bool hasInfiniteHealth = false;  // Indica si tiene vida infinita activada
};

enum class GameState { PLAYING, WON, LOST };

enum class EndScreenPhase { RACE_RESULTS, ACCUMULATED_RESULTS };

class Game {
private:
    ClientSession& client_session;
    SDL2pp::Renderer* rendererPtr = nullptr;
    uint32_t client_id = UINT32_MAX;
    Camera camera;
    Minimap minimap;
    HUD hud;
    CheckpointArrow arrow;
    CollisionExplosion explosion;
    CarFireEffect fireEffect;
    CarSoundEngine carSoundEngine;
    std::map<int, std::shared_ptr<SDL2pp::Texture>> textures;
    std::map<uint8_t, std::shared_ptr<SDL2pp::Texture>>
            carTextures;  // Texturas individuales por auto

    std::vector<CarSnapshot> snapshots;
    std::vector<RenderCar> carsToRender;
    EndScreenPhase endScreenPhase = EndScreenPhase::RACE_RESULTS;

    float testPlayerX = 350.0f;
    float testPlayerY = 300.0f;
    float testPlayerAngle = 0.0f;
    bool showMinimap = true;
    uint8_t currentCityId = 0;
    int currentCheckpoint = 0;
    int totalCheckpoints = 4;
    float elapsedTime = 0.0f;

    std::vector<RaceCheckpoint> trackCheckpoints;

    std::unordered_map<int, float> previousHealthState;

    float lastPlayerX = 0.0f;
    float lastPlayerY = 0.0f;
    Uint32 lastSpeedUpdateTime = 0;

    Uint32 collisionFlashStartTime = 0;
    const Uint32 FLASH_DURATION_MS = 300;
    float lastCollisionIntensity = 0.0f;

    bool playerDestroyed = false;
    Uint32 destructionStartTime = 0;

    // Countdown system
    bool showCountdown = false;
    uint8_t countdownValue = 0;
    Uint32 yaStartTime = 0;

    GameState gameState = GameState::PLAYING;
    std::string endGameMessage = "";
    Uint32 endGameTime = 0;

    std::vector<ClientPlayerResult> raceResults;
    bool hasRaceResults = false;
    bool raceFullyFinished = false;
    std::vector<AccumulatedResultDTO> accumulatedResults;
    bool isLastRace = false;
    ClientPlayerResult myOwnResults = {0, "", 0.0f, 0};
    std::map<uint32_t, std::string> playerNames;  // Mapeo de client_id a playerName

    // Upgrades system
    bool showUpgradesScreen = false;
    Uint32 upgradesScreenStartTime = 0;
    const Uint32 UPGRADES_SCREEN_DURATION_MS = 10000;  // 10 segundos
    CarUpgrades currentUpgrades;
    CarUpgrades selectedUpgrades;

    SDL_Rect src;
    SDL_Rect dst;

    bool handleEvents();

    void init_textures();

    bool update(ServerToClientSnapshot_Client cmd_snapshot);

    void render();

    void renderEndGameScreen();

    void setWon();

    void setLost();

    float getScale(int w, int h) const;

    void renderPressESC();

    void renderMyOwnTime();

    void renderRaceTable();

    SDL_Rect getSpriteForCarId(uint8_t car_id);

    void renderAccumulatedTable();

    void initMinimapAndCheckpoints(const std::string& trackName);

    void renderCheckpoints(SDL2pp::Renderer& renderer);

    void renderUpgradesScreen();

    void handleUpgradesInput(const SDL_Event& event);

    void renderCountdown();

    bool isCountdownActive() const;

    void renderOwnName(const SDL_Rect& rowRect) const;

    void renderPlayerNames(SDL2pp::Renderer& renderer, const SDL_Rect& src, float scale);

public:
    explicit Game(ClientSession& client_session);

    int start();
    void setClientId(uint32_t id);
    void update_snapshots(const std::vector<CarSnapshot>& snapshots, float elapsedTime);
    void setRaceResults(const std::vector<ClientPlayerResult>& results, bool isFinished);
    void setIsLastRace(bool lastRace) { isLastRace = lastRace; }
    void setAccumulatedResults(const std::vector<AccumulatedResultDTO>& res);
    void setCountdownValue(uint8_t value);
    void resetForNextRace(uint8_t nextCityId, const std::string& trackName);
};

#endif
