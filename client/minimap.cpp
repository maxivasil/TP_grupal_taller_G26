#include "minimap.h"
#include <iostream>
#include <cmath>

static inline int clampi(int v, int lo, int hi) {
    return std::max(lo, std::min(v, hi));
}

Minimap::Minimap(int size)
    : size(size),
      arrowPixelSize(8),
      mapWidth(0.0f),
      mapHeight(0.0f),
      scaleX(1.0f),
      scaleY(1.0f),
      offsetX(0.0f),
      offsetY(0.0f) {}

Minimap::~Minimap() = default;

void Minimap::loadMapImage(SDL2pp::Renderer& renderer, const std::string& imagePath) {
    try {
        mapTexture = std::make_unique<SDL2pp::Texture>(renderer, imagePath);
        // Tomar dimensiones reales del PNG
        mapWidth  = static_cast<float>(mapTexture->GetWidth());
        mapHeight = static_cast<float>(mapTexture->GetHeight());

        std::cout << "Minimap: Loaded map image " << imagePath
                  << " (" << mapWidth << "x" << mapHeight << ")\n";

    } catch (const std::exception& e) {
        std::cerr << "Error loading minimap image: " << e.what() << "\n";
        mapTexture = nullptr;
        mapWidth = mapHeight = 0.0f;
    }
}

void Minimap::setCheckpoints(const std::vector<RaceCheckpoint>& cp) {
    checkpoints = cp;
    std::cout << "Minimap: Loaded " << checkpoints.size() << " checkpoints\n";
}

int Minimap::worldToMinimapX(float serverX) const {
    if (mapWidth <= 0.0f) return 0;
    // server -> pixel mapa
    float px = serverX * scaleX + offsetX;
    // pixel mapa -> pixel minimapa (dest se escala no-uniformemente a size x size)
    int mx = static_cast<int>((px / mapWidth) * size);
    return clampi(mx, 0, size - 1);
}

int Minimap::worldToMinimapY(float serverY) const {
    if (mapHeight <= 0.0f) return 0;
    float py = serverY * scaleY + offsetY;
    int my = static_cast<int>((py / mapHeight) * size);
    return clampi(my, 0, size - 1);
}

void Minimap::renderCheckpoints(SDL2pp::Renderer& renderer) {
    int screenX = renderer.GetOutputWidth() - size - 10;
    int screenY = 10;

    for (const auto& cp : checkpoints) {
        int minimapX = worldToMinimapX(cp.x);
        int minimapY = worldToMinimapY(cp.y);

        int screenMinimapX = screenX + minimapX;
        int screenMinimapY = screenY + minimapY;

        if (cp.isFinish) {
            renderer.SetDrawColor(255, 50, 50, 255);  // Rojo para finish
        } else {
            renderer.SetDrawColor(100, 255, 100, 255);  // Verde para checkpoints
        }

        int radius = 3;
        SDL_Rect rect{screenMinimapX - radius, screenMinimapY - radius, radius * 2, radius * 2};
        renderer.FillRect(rect);
    }
}

void Minimap::renderPlayer(SDL2pp::Renderer& renderer, const MinimapPlayer& p, bool isLocal) {
    int screenX = renderer.GetOutputWidth() - size - 10;
    int screenY = 10;

    int minimapX = worldToMinimapX(p.x);
    int minimapY = worldToMinimapY(p.y);

    int screenMinimapX = screenX + minimapX;
    int screenMinimapY = screenY + minimapY;

    if (isLocal) {
        renderer.SetDrawColor(255, 255, 255, 255);

        float angleRad = p.angle * 3.14159265f / 180.0f;
        float cos_a = std::cos(angleRad);
        float sin_a = std::sin(angleRad);

        int r = arrowPixelSize;
        // Flecha centrada que rota con el ángulo
        SDL2pp::Point pts[3] = {
            SDL2pp::Point(screenMinimapX + static_cast<int>(r * cos_a),
                          screenMinimapY + static_cast<int>(r * sin_a)),
            SDL2pp::Point(screenMinimapX - static_cast<int>(r * sin_a),
                          screenMinimapY + static_cast<int>(r * cos_a)),
            SDL2pp::Point(screenMinimapX + static_cast<int>(r * sin_a),
                          screenMinimapY - static_cast<int>(r * cos_a))
        };

        renderer.DrawLines(pts, 3);
        renderer.DrawLine(pts[2], pts[0]);

        // Punto amarillo en la punta
        renderer.SetDrawColor(255, 255, 0, 255);
        SDL_Rect dot{pts[0].x - 2, pts[0].y - 2, 4, 4};
        renderer.FillRect(dot);

    } else {
        renderer.SetDrawColor(100, 200, 255, 255);
        SDL_Rect rect{screenMinimapX - 2, screenMinimapY - 2, 4, 4};
        renderer.FillRect(rect);
    }
}

void Minimap::render(SDL2pp::Renderer& renderer,
                     const MinimapPlayer& localPlayer,
                     const std::vector<MinimapPlayer>& otherPlayers) {
    int screenX = renderer.GetOutputWidth() - size - 10;
    int screenY = 10;

    // Fondo oscuro + borde
    renderer.SetDrawColor(20, 20, 30, 255);
    SDL_Rect bg{screenX - 2, screenY - 2, size + 4, size + 4};
    renderer.FillRect(bg);

    // Dibujar el mapa (se estira a size x size; usamos mapWidth/mapHeight para escalar coords)
    if (mapTexture) {
        SDL_Rect dst{screenX, screenY, size, size};
        renderer.Copy(*mapTexture, SDL2pp::NullOpt, dst);
    } else {
        renderer.SetDrawColor(100, 100, 100, 255);
        SDL_Rect placeholder{screenX, screenY, size, size};
        renderer.FillRect(placeholder);
    }

    // Checkpoints y jugadores
    renderCheckpoints(renderer);
    for (const auto& player : otherPlayers) {
        renderPlayer(renderer, player, false);
    }
    renderPlayer(renderer, localPlayer, true);

    // Marco cian
    renderer.SetDrawColor(0, 255, 255, 255);
    for (int i = 0; i < 2; i++) {
        SDL_Rect border{screenX - 2 - i, screenY - 2 - i, size + 4 + (i * 2), size + 4 + (i * 2)};
        renderer.DrawRect(border);
    }

    // Debug (cada 60 frames)
    static int frame = 0;
    if (frame++ % 60 == 0) {
        std::cout << "Minimap: map=" << mapWidth << "x" << mapHeight
                  << " scale=(" << scaleX << "," << scaleY << ")"
                  << " offset=(" << offsetX << "," << offsetY << ")\n";
    }
}