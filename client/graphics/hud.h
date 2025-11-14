#ifndef CLIENT_HUD_H
#define CLIENT_HUD_H

#include <string>

#include <SDL2pp/SDL2pp.hh>

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
    std::string loadedFontPath;

public:
    explicit HUD(int windowWidth, int windowHeight);
    ~HUD();

    HUD(const HUD&) = delete;
    HUD& operator=(const HUD&) = delete;
    HUD(HUD&&) = default;
    HUD& operator=(HUD&&) = default;

    void loadFont(const std::string& fontPath, int fontSize);
    void render(SDL2pp::Renderer& renderer, const HUDData& data);

    const std::string& getFontPath() const { return loadedFontPath; }

public:
    // Acceso público para obtener fontPath desde el exterior
    std::string fontPath;
};

#endif
