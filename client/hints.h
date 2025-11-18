#ifndef CLIENT_HINTS_H
#define CLIENT_HINTS_H

#include <cmath>
#include <memory>
#include <string>

#include <SDL2pp/SDL2pp.hh>

struct HintData {
    float angle;     // Ángulo desde auto al checkpoint (-pi a pi)
    float distance;  // Distancia en unidades del mundo
};

class Hints {
private:
    int windowWidth;
    int windowHeight;

    // Posición del compass HUD (esquina superior derecha)
    int compassX;
    int compassY;
    int compassRadius = 45;  // Radio del compass circular

    HintData currentHint;
    bool hasHint = false;

    // Font support from HUD
    SDL2pp::Font* hudFont = nullptr;
    std::string fontPath;
    int fontSize = 12;

    // Player heading for camera-relative needle
    float playerHeading = 0.0f;  // Player's rotation angle

    // Animation state
    Uint32 animationStartTime = 0;
    bool isAnimating = false;

    // Pulse animation (for elegance)
    float pulsePhase = 0.0f;  // 0 to 1 cycle

public:
    explicit Hints(int windowWidth, int windowHeight);
    ~Hints() = default;

    Hints(const Hints&) = delete;
    Hints& operator=(const Hints&) = delete;
    Hints(Hints&&) = default;
    Hints& operator=(Hints&&) = default;

    // Set the HUD font for text rendering (call after HUD font is loaded)
    void setHUDFont(SDL2pp::Font* font, const std::string& path) {
        hudFont = font;
        fontPath = path;
    }

    // Actualizar la posición del hint basado en auto y checkpoint
    void updateHint(float playerX, float playerY, float checkpointX, float checkpointY,
                    float playerHeadingAngle = 0.0f);

    // Renderizar compass HUD elegante
    void render(SDL2pp::Renderer& renderer);

    // Getters
    bool hasActiveHint() const { return hasHint; }
    float getDistanceToCheckpoint() const { return currentHint.distance; }

private:
    // Dibujar brújula
    void drawCompass(SDL2pp::Renderer& renderer);

    // Dibujar línea gruesa
    void drawThickLine(SDL2pp::Renderer& renderer, int x1, int y1, int x2, int y2, int thickness,
                       SDL_Color color);

    // Dibujar círculo relleno
    void drawFilledCircle(SDL2pp::Renderer& renderer, int x, int y, int radius, SDL_Color color);

    // Get current pulse opacity (0.6 to 1.0)
    float getPulseOpacity() const;
};

#endif
