#include "hud.h"
#include <iostream>
#include <iomanip>
#include <sstream>

HUD::HUD(int windowWidth, int windowHeight) 
    : windowWidth(windowWidth), windowHeight(windowHeight), font(nullptr) {}

HUD::~HUD() {
    // Font is managed by SDL2pp, no manual cleanup needed
}

void HUD::loadFont(const std::string& fontPath, int fontSize) {
    try {
        font = new SDL2pp::Font(fontPath, fontSize);
        fontLoaded = true;
        this->fontPath = fontPath;
        std::cout << "HUD: Font loaded successfully from " << fontPath << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "HUD: Failed to load font: " << e.what() << std::endl;
        fontLoaded = false;
    }
}

void HUD::render(SDL2pp::Renderer& renderer, const HUDData& data) {
    if (!fontLoaded || !font) {
        return;  // No font, can't render text
    }

    Uint8 r, g, b, a;
    renderer.GetDrawColor(r, g, b, a);
    try {
        // Prepare text for display
        std::ostringstream speedText;
        speedText << std::fixed << std::setprecision(1) << "Speed: " << data.speed << " m/s";
        
        std::ostringstream healthText;
        healthText << std::fixed << std::setprecision(0) << "Health: " << data.health << "%";
        
        std::ostringstream checkpointText;
        checkpointText << "Checkpoint: " << data.checkpointCurrent << "/" << data.checkpointTotal;
        
        int minutes = static_cast<int>(data.raceTime) / 60;
        int seconds = static_cast<int>(data.raceTime) % 60;
        std::ostringstream timeText;
        timeText << "Time: " << minutes << ":" << std::setfill('0') << std::setw(2) << seconds;

        // Render text surfaces using font's RenderText_Solid method
        SDL2pp::Surface speedSurface = font->RenderText_Solid(speedText.str(), SDL_Color{0, 255, 0, 255});
        SDL2pp::Surface healthSurface = font->RenderText_Solid(healthText.str(), SDL_Color{255, 0, 0, 255});
        SDL2pp::Surface checkpointSurface = font->RenderText_Solid(checkpointText.str(), SDL_Color{0, 255, 255, 255});
        SDL2pp::Surface timeSurface = font->RenderText_Solid(timeText.str(), SDL_Color{255, 255, 255, 255});

        // Convert surfaces to textures
        SDL2pp::Texture speedTexture(renderer, speedSurface);
        SDL2pp::Texture healthTexture(renderer, healthSurface);
        SDL2pp::Texture checkpointTexture(renderer, checkpointSurface);
        SDL2pp::Texture timeTexture(renderer, timeSurface);

        // Position: top-left corner with padding
        int padding = 10;
        int lineHeight = 25;
        int x = padding;
        int y = padding;
        
        // Calculate the bounding box for all text (to draw background)
        int maxWidth = std::max({speedSurface.GetWidth(), healthSurface.GetWidth(), 
                                 checkpointSurface.GetWidth(), timeSurface.GetWidth()});
        int totalHeight = lineHeight * 4;  // 4 lines of text
        int bgPadding = 8;  // Extra padding around text for background
        
        // Draw black background rectangle
        renderer.SetDrawColor(0, 0, 0, 200);  // Black with slight transparency
        SDL_Rect bgRect{x - bgPadding, y - bgPadding, maxWidth + (bgPadding * 2), totalHeight + bgPadding};
        renderer.FillRect(bgRect);
        
        // Draw border around background
        renderer.SetDrawColor(255, 255, 255, 150);  // White border
        renderer.DrawRect(bgRect);

        // Render speed
        renderer.Copy(speedTexture, SDL2pp::NullOpt, 
                     SDL2pp::Rect(x, y, speedSurface.GetWidth(), speedSurface.GetHeight()));
        y += lineHeight;

        // Render health
        renderer.Copy(healthTexture, SDL2pp::NullOpt,
                     SDL2pp::Rect(x, y, healthSurface.GetWidth(), healthSurface.GetHeight()));
        y += lineHeight;

        // Render checkpoint
        renderer.Copy(checkpointTexture, SDL2pp::NullOpt,
                     SDL2pp::Rect(x, y, checkpointSurface.GetWidth(), checkpointSurface.GetHeight()));
        y += lineHeight;

        // Render time
        renderer.Copy(timeTexture, SDL2pp::NullOpt,
                     SDL2pp::Rect(x, y, timeSurface.GetWidth(), timeSurface.GetHeight()));
    } catch (const std::exception& e) {
        std::cerr << "HUD: Error rendering text: " << e.what() << std::endl;
    }
    renderer.SetDrawColor(r, g, b, a);
}
