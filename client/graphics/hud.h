#ifndef CLIENT_HUD_H
#define CLIENT_HUD_H

#include <string>

#include <SDL2pp/SDL2pp.hh>

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
    bool fontLoaded = false;
    std::string loadedFontPath;
    float hudScale = 1.0f;
    AudioState audioState = AudioState::FULL_SOUND;  // Track audio state

    // Mute button properties
    SDL_Rect muteButtonRect{0, 0, 120, 40};  // Position and size will be set on resize

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

    void onWindowResize(int w, int h, float scale);

    // Acceso público para obtener fontPath desde el exterior
    std::string fontPath;

    // Audio state tracking
    void setAudioState(AudioState state) { audioState = state; }
    AudioState getAudioState() const { return audioState; }

    // Check if mute button was clicked
    bool isMuteButtonClicked(int mouseX, int mouseY) const;
    const SDL_Rect& getMuteButtonRect() const { return muteButtonRect; }
};

#endif
