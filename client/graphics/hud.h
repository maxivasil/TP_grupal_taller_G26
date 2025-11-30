#ifndef CLIENT_HUD_H
#define CLIENT_HUD_H

#include <string>
#include <vector>

#include <SDL2pp/SDL2pp.hh>

#include "../../common/constants.h"
#include "../audio/car_sound_engine.h"

struct HUDData {
    float speed;
    float health;
    int checkpointCurrent;
    int checkpointTotal;
    float raceTime;  // en segundos
};

class HUD {
private:
    int windowWidth;
    int windowHeight;
    SDL2pp::Font* font;
    float hudScale = 1.0f;

public:
    explicit HUD(int windowWidth, int windowHeight);
    ~HUD();

    HUD(const HUD&) = delete;
    HUD& operator=(const HUD&) = delete;
    HUD(HUD&&) = default;
    HUD& operator=(HUD&&) = default;

    void initFont(const std::string& fontPath, int fontSize);

    void render(SDL2pp::Renderer& renderer, const HUDData& data);

    void onWindowResize(int w, int h, float scale);
};

#endif
