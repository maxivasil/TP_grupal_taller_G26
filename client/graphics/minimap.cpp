#include "minimap.h"

#include <cmath>
#include <iostream>

static inline int clampi(int v, int lo, int hi) { return std::max(lo, std::min(v, hi)); }

Minimap::Minimap(int size):
        size(size),
        arrowPixelSize(8),
        mapWidth(0.0f),
        mapHeight(0.0f),
        scaleX(1.0f),
        scaleY(1.0f),
        offsetX(0.0f),
        offsetY(0.0f),
        zoomPixelWidth(800.0f),
        zoomPixelHeight(800.0f),
        viewLeft(0.0f),
        viewTop(0.0f) {}

Minimap::~Minimap() = default;

void Minimap::loadMapImage(SDL2pp::Renderer& renderer, const std::string& imagePath) {
    try {
        mapTexture = std::make_unique<SDL2pp::Texture>(renderer, imagePath);
        mapWidth = static_cast<float>(mapTexture->GetWidth());
        mapHeight = static_cast<float>(mapTexture->GetHeight());
    } catch (const std::exception& e) {
        std::cerr << "Error loading minimap image: " << e.what() << "\n";
        mapTexture = nullptr;
        mapWidth = mapHeight = 0.0f;
    }
}

void Minimap::setWorldScale(float sx, float sy) {
    scaleX = sx;
    scaleY = sy;
}

void Minimap::setZoomPixels(float w, float h) {
    zoomPixelWidth = std::max(50.0f, w);
    zoomPixelHeight = std::max(50.0f, h);
}

void Minimap::setCheckpoints(const std::vector<RaceCheckpoint>& cp) { checkpoints = cp; }

void Minimap::updateViewport(float playerMapX, float playerMapY) {
    if (mapWidth <= 0.0f || mapHeight <= 0.0f) {
        viewLeft = viewTop = 0.0f;
        return;
    }
    float halfW = zoomPixelWidth * 0.5f;
    float halfH = zoomPixelHeight * 0.5f;

    float left = playerMapX - halfW;
    float top = playerMapY - halfH;

    if (left < 0.0f)
        left = 0.0f;
    if (top < 0.0f)
        top = 0.0f;

    if (left + zoomPixelWidth > mapWidth)
        left = mapWidth - zoomPixelWidth;
    if (top + zoomPixelHeight > mapHeight)
        top = mapHeight - zoomPixelHeight;

    viewLeft = left;
    viewTop = top;
}

int Minimap::worldToMinimapX(float serverX) const {
    if (mapWidth <= 0.0f)
        return 0;
    float px = serverX * scaleX + offsetX;         // píxel absoluto del mapa
    float rel = (px - viewLeft) / zoomPixelWidth;  // normalizado respecto al viewport
    int mx = static_cast<int>(rel * size);
    return clampi(mx, 0, size - 1);
}

int Minimap::worldToMinimapY(float serverY) const {
    if (mapHeight <= 0.0f)
        return 0;
    float py = serverY * scaleY + offsetY;
    float rel = (py - viewTop) / zoomPixelHeight;
    int my = static_cast<int>(rel * size);
    return clampi(my, 0, size - 1);
}

void Minimap::renderCheckpoints(SDL2pp::Renderer& renderer, int nextCheckpointId) {
    int screenX = renderer.GetOutputWidth() - size - 10;

    if (nextCheckpointId < (int)checkpoints.size()) {
        const auto& cp = checkpoints[nextCheckpointId];
        int minimapX = worldToMinimapX(cp.x);
        int minimapY = worldToMinimapY(cp.y);

        constexpr int screenY = 10;
        int screenMinimapX = screenX + minimapX;
        int screenMinimapY = screenY + minimapY;

        int radius = 7;

        if (cp.isFinish) {
            renderer.SetDrawColor(255, 50, 50, 255);
        } else {
            renderer.SetDrawColor(0, 255, 0, 255);
        }

        SDL_Rect rect{screenMinimapX - radius, screenMinimapY - radius, radius * 2, radius * 2};
        renderer.FillRect(rect);

        renderer.SetDrawColor(255, 255, 0, 255);
        for (int j = 0; j < 3; ++j) {
            SDL_Rect borderRect{screenMinimapX - radius - j, screenMinimapY - radius - j,
                                (radius + j) * 2, (radius + j) * 2};
            renderer.DrawRect(borderRect);
        }
    }
}

void Minimap::renderPlayer(SDL2pp::Renderer& renderer, const MinimapPlayer& p) {
    int screenX = renderer.GetOutputWidth() - size - 10;
    int screenY = 10;

    int minimapX = worldToMinimapX(p.x);
    int minimapY = worldToMinimapY(p.y);

    int screenMinimapX = screenX + minimapX;
    int screenMinimapY = screenY + minimapY;

    renderer.SetDrawColor(255, 255, 255, 255);
    float angleRad = p.angle * 3.14159265f / 180.0f;
    float cos_a = std::cos(angleRad);
    float sin_a = std::sin(angleRad);
    int r = arrowPixelSize;
    SDL2pp::Point pts[3] = {
            SDL2pp::Point(screenMinimapX + int(r * cos_a), screenMinimapY + int(r * sin_a)),
            SDL2pp::Point(screenMinimapX - int(r * sin_a), screenMinimapY + int(r * cos_a)),
            SDL2pp::Point(screenMinimapX + int(r * sin_a), screenMinimapY - int(r * cos_a))};
    renderer.DrawLines(pts, 3);
    renderer.DrawLine(pts[2], pts[0]);
    renderer.SetDrawColor(255, 255, 0, 255);
    SDL_Rect dot{pts[0].x - 2, pts[0].y - 2, 4, 4};
    renderer.FillRect(dot);
}

void Minimap::render(SDL2pp::Renderer& renderer, const MinimapPlayer& localPlayer,
                     int nextCheckpointId) {
    Uint8 r, g, b, a;
    renderer.GetDrawColor(r, g, b, a);
    int screenX = renderer.GetOutputWidth() - size - 10;
    int screenY = 10;

    renderer.SetDrawColor(20, 20, 30, 255);
    SDL_Rect bg{screenX - 2, screenY - 2, size + 4, size + 4};
    renderer.FillRect(bg);

    float playerMapX = localPlayer.x * scaleX + offsetX;
    float playerMapY = localPlayer.y * scaleY + offsetY;
    updateViewport(playerMapX, playerMapY);

    if (mapTexture) {
        SDL_Rect src{int(viewLeft), int(viewTop), int(std::min(zoomPixelWidth, mapWidth)),
                     int(std::min(zoomPixelHeight, mapHeight))};
        SDL_Rect dst{screenX, screenY, size, size};
        renderer.Copy(*mapTexture, src, dst);
    } else {
        renderer.SetDrawColor(100, 100, 100, 255);
        SDL_Rect placeholder{screenX, screenY, size, size};
        renderer.FillRect(placeholder);
    }

    renderCheckpoints(renderer, nextCheckpointId);
    renderPlayer(renderer, localPlayer);

    renderer.SetDrawColor(0, 255, 255, 255);
    for (int i = 0; i < 2; i++) {
        SDL_Rect border{screenX - 2 - i, screenY - 2 - i, size + 4 + (i * 2), size + 4 + (i * 2)};
        renderer.DrawRect(border);
    }

    renderer.SetDrawColor(r, g, b, a);
}

void Minimap::onWindowResize(int w, int h, float scale) {
    size = int(150 * scale);
    arrowPixelSize = int(8 * scale);
    zoomPixelWidth = 900 * scale;
    zoomPixelHeight = 900 * scale;
}
