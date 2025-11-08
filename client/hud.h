#ifndef CLIENT_HUD_H
#define CLIENT_HUD_H

#include <SDL2pp/SDL2pp.hh>
#include <string>

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
    bool fontLoaded = false;

public:
    explicit HUD(int windowWidth, int windowHeight);
    ~HUD();
    
    HUD(const HUD&) = delete;
    HUD& operator=(const HUD&) = delete;
    HUD(HUD&&) = default;
    HUD& operator=(HUD&&) = default;
    
    void loadFont(const std::string& fontPath, int fontSize);
    void render(SDL2pp::Renderer& renderer, const HUDData& data);
};

#endif
