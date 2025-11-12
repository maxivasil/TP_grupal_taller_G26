// Enhanced compass-style HUD with animation, font integration, and camera-relative needle
#include "hints.h"
#include <cmath>
#include <algorithm>
#include <iostream>

Hints::Hints(int windowWidth, int windowHeight)
    : windowWidth(windowWidth), windowHeight(windowHeight),
      compassX(windowWidth / 2), compassY(70), compassRadius(45),
      currentHint({0.0f, 0.0f}), hasHint(false),
      playerHeading(0.0f), animationStartTime(SDL_GetTicks()),
      isAnimating(true), pulsePhase(0.0f) {}

void Hints::updateHint(float playerX, float playerY,
                       float checkpointX, float checkpointY,
                       float playerHeadingAngle) {
    // If checkpoint undefined, disable
    if (checkpointX == 0.0f && checkpointY == 0.0f) {
        hasHint = false;
        return;
    }

    float dx = checkpointX - playerX;
    float dy = checkpointY - playerY;
    float distance = std::sqrt(dx * dx + dy * dy);
    float absoluteAngle = std::atan2(dy, dx);  // angle in radians, 0 = +X

    currentHint.distance = distance;
    currentHint.angle = absoluteAngle;
    playerHeading = playerHeadingAngle;

    hasHint = (distance > 0.5f);
    
    // Update pulse animation
    Uint32 elapsed = SDL_GetTicks() - animationStartTime;
    pulsePhase = fmod(elapsed / 1000.0f * 2.0f, 2.0f);  // 2-second cycle
}

float Hints::getPulseOpacity() const {
    // Creates a smooth 0.6->1.0->0.6 pulsing effect over 2 seconds
    float phase = pulsePhase;
    if (phase < 1.0f) {
        return 0.6f + 0.4f * phase;  // 0.6 to 1.0
    } else {
        return 1.0f + 0.4f * (phase - 1.0f);  // 1.0 to 0.6
    }
}

void Hints::drawThickLine(SDL2pp::Renderer& renderer,
                          int x1, int y1, int x2, int y2,
                          int thickness, SDL_Color color) {
    renderer.SetDrawColor(color);

    float dx = float(x2 - x1);
    float dy = float(y2 - y1);
    float len = std::sqrt(dx * dx + dy * dy);
    if (len < 1e-6f) {
        renderer.DrawPoint(x1, y1);
        return;
    }

    float px = -dy / len; // perpendicular
    float py = dx / len;

    for (int i = -thickness/2; i <= thickness/2; ++i) {
        float ox = px * i;
        float oy = py * i;
        renderer.DrawLine(
            static_cast<int>(x1 + ox), static_cast<int>(y1 + oy),
            static_cast<int>(x2 + ox), static_cast<int>(y2 + oy)
        );
    }
}

void Hints::drawFilledCircle(SDL2pp::Renderer& renderer,
                             int cx, int cy, int radius, SDL_Color color) {
    renderer.SetDrawColor(color);
    for (int dx = -radius; dx <= radius; ++dx) {
        int h = static_cast<int>(std::sqrt(std::max(0, radius*radius - dx*dx)));
        renderer.DrawLine(cx + dx, cy - h, cx + dx, cy + h);
    }
}

void Hints::drawCompass(SDL2pp::Renderer& renderer) {
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    
    // Get pulse opacity for smooth animation
    float pulseOp = getPulseOpacity();
    int bgOpacity = static_cast<int>(160 * pulseOp);
    int ringOpacity = static_cast<int>(220 * pulseOp);
    int tickOpacity = static_cast<int>(200 * pulseOp);
    
    // Background semi-transparent circle (dark with pulse)
    drawFilledCircle(renderer, compassX, compassY, compassRadius, 
                    {10, 10, 10, static_cast<Uint8>(bgOpacity)});

    // Outer ring (blue) with pulsing
    renderer.SetDrawColor({100, 150, 255, static_cast<Uint8>(ringOpacity)});
    for (int r = compassRadius - 2; r <= compassRadius; ++r) {
        int steps = 64;
        for (int i = 0; i < steps; ++i) {
            float a1 = (float(i) / steps) * 2.0f * M_PI;
            float a2 = (float(i+1) / steps) * 2.0f * M_PI;
            int x1 = compassX + static_cast<int>(r * std::cos(a1));
            int y1 = compassY + static_cast<int>(r * std::sin(a1));
            int x2 = compassX + static_cast<int>(r * std::cos(a2));
            int y2 = compassY + static_cast<int>(r * std::sin(a2));
            renderer.DrawLine(x1, y1, x2, y2);
        }
    }

    // Cardinal ticks (N, E, S, W) with pulsing
    renderer.SetDrawColor({200, 200, 200, static_cast<Uint8>(tickOpacity)});
    int tickLen = 8;
    renderer.DrawLine(compassX, compassY - compassRadius, compassX, compassY - compassRadius - tickLen);  // N
    renderer.DrawLine(compassX + compassRadius, compassY, compassX + compassRadius + tickLen, compassY);  // E
    renderer.DrawLine(compassX, compassY + compassRadius, compassX, compassY + compassRadius + tickLen);  // S
    renderer.DrawLine(compassX - compassRadius, compassY, compassX - compassRadius - tickLen, compassY);  // W
    
    // Cardinal letters (N, E, S, W) if font is available
    if (hudFont) {
        try {
            SDL_Color labelColor = {220, 220, 220, static_cast<Uint8>(tickOpacity)};
            
            // N - North
            auto nSurf = hudFont->RenderText_Solid("N", labelColor);
            SDL2pp::Texture nTex(renderer, nSurf);
            int nx = compassX - nTex.GetWidth() / 2;
            int ny = compassY - compassRadius - tickLen - 12;
            SDL_Rect nDst = {nx, ny, nTex.GetWidth(), nTex.GetHeight()};
            renderer.Copy(nTex, SDL2pp::NullOpt, nDst);
            
            // E - East
            auto eSurf = hudFont->RenderText_Solid("E", labelColor);
            SDL2pp::Texture eTex(renderer, eSurf);
            int ex = compassX + compassRadius + tickLen + 4;
            int ey = compassY - eTex.GetHeight() / 2;
            SDL_Rect eDst = {ex, ey, eTex.GetWidth(), eTex.GetHeight()};
            renderer.Copy(eTex, SDL2pp::NullOpt, eDst);
            
            // S - South
            auto sSurf = hudFont->RenderText_Solid("S", labelColor);
            SDL2pp::Texture sTex(renderer, sSurf);
            int sx = compassX - sTex.GetWidth() / 2;
            int sy = compassY + compassRadius + tickLen + 2;
            SDL_Rect sDst = {sx, sy, sTex.GetWidth(), sTex.GetHeight()};
            renderer.Copy(sTex, SDL2pp::NullOpt, sDst);
            
            // W - West
            auto wSurf = hudFont->RenderText_Solid("W", labelColor);
            SDL2pp::Texture wTex(renderer, wSurf);
            int wx = compassX - compassRadius - tickLen - wTex.GetWidth() - 4;
            int wy = compassY - wTex.GetHeight() / 2;
            SDL_Rect wDst = {wx, wy, wTex.GetWidth(), wTex.GetHeight()};
            renderer.Copy(wTex, SDL2pp::NullOpt, wDst);
        } catch (const std::exception& e) {
            // Font rendering failed, continue without labels
        }
    }
}

void Hints::render(SDL2pp::Renderer& renderer) {
    if (!hasHint) return;
    Uint8 r, g, b, a;
    renderer.GetDrawColor(r, g, b, a);

    // Draw compass HUD at fixed position (top-right area)
    drawCompass(renderer);

    // Compute needle angle:
    // - If playerHeading is used: needle is relative to player's heading
    // - Otherwise: absolute north (world space)
    float arrowAngle;
    
    if (playerHeading != 0.0f) {
        // Camera-relative: subtract player heading to make needle relative to car direction
        arrowAngle = currentHint.angle - playerHeading - (M_PI / 2.0f);
    } else {
        // Absolute: 0 = +X (east), rotate to 0 = -Y (up/north)
        arrowAngle = currentHint.angle - (M_PI / 2.0f);
    }

    int arrowLen = static_cast<int>(compassRadius * 0.65f);
    int endX = compassX + static_cast<int>(arrowLen * std::cos(arrowAngle));
    int endY = compassY + static_cast<int>(arrowLen * std::sin(arrowAngle));

    // Draw arrow (red needle) with pulsing opacity
    float pulseOp = getPulseOpacity();
    int needleOpacity = static_cast<int>(255 * pulseOp);
    SDL_Color needleColor = {255, 80, 80, static_cast<Uint8>(needleOpacity)};
    drawThickLine(renderer, compassX, compassY, endX, endY, 3, needleColor);

    // Draw triangular head
    float headA1 = arrowAngle + 0.9f;
    float headA2 = arrowAngle - 0.9f;
    int hx1 = endX + static_cast<int>(10 * std::cos(headA1));
    int hy1 = endY + static_cast<int>(10 * std::sin(headA1));
    int hx2 = endX + static_cast<int>(10 * std::cos(headA2));
    int hy2 = endY + static_cast<int>(10 * std::sin(headA2));

    renderer.SetDrawColor(needleColor);
    renderer.DrawLine(endX, endY, hx1, hy1);
    renderer.DrawLine(endX, endY, hx2, hy2);
    renderer.DrawLine(hx1, hy1, hx2, hy2);

    // Small center cap (white)
    drawFilledCircle(renderer, compassX, compassY, 4, {255, 255, 255, static_cast<Uint8>(needleOpacity)});

    // Distance text: use HUD font if available, otherwise fallback
    try {
        SDL2pp::Font* fontToUse = hudFont;
        SDL2pp::Font fallbackFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 11);
        
        if (!fontToUse) {
            fontToUse = &fallbackFont;
        }
        
        char buf[32];
        snprintf(buf, sizeof(buf), "%.1f m", currentHint.distance);
        SDL_Color textColor = {220, 220, 220, static_cast<Uint8>(needleOpacity)};
        auto surf = fontToUse->RenderText_Solid(buf, textColor);
        SDL2pp::Texture tex(renderer, surf);
        int tx = compassX - tex.GetWidth() / 2;
        int ty = compassY + compassRadius + 12;
        SDL_Rect dst = {tx, ty, tex.GetWidth(), tex.GetHeight()};
        renderer.Copy(tex, SDL2pp::NullOpt, dst);
    } catch (const std::exception& e) {
        // Font not available or rendering failed: skip distance text
    }

    // Restore blend mode
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
    renderer.SetDrawColor(r, g, b, a);
}
