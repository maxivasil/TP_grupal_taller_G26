#ifndef CLIENT_MINIMAP_H
#define CLIENT_MINIMAP_H

#include <SDL2pp/SDL2pp.hh>
#include <memory>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

struct RaceCheckpoint {
    int id;
    float x, y, width, height;
    bool isFinish;
};

struct MinimapPlayer {
    float x, y, angle;  // Coordenadas del servidor
    int playerId;
    float health;
    bool isLocal;
};

class Minimap {
private:
    int size;  // tamaño cuadrado del minimapa en píxeles
    int arrowPixelSize = 8;

    std::unique_ptr<SDL2pp::Texture> mapTexture;

    std::vector<RaceCheckpoint> checkpoints;

    // Dimensiones reales del PNG del mapa
    float mapWidth = 0.0f;
    float mapHeight = 0.0f;

    // Transformación servidor -> píxeles del mapa
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float offsetX = 0.0f;
    float offsetY = 0.0f;

    // Viewport (sub-rectángulo dentro del mapa) centrado en el jugador
    float zoomPixelWidth  = 800.0f;  // ancho del área visible del mundo en píxeles del mapa
    float zoomPixelHeight = 800.0f;  // alto del área visible
    float viewLeft = 0.0f;
    float viewTop  = 0.0f;

    // Calcula y clampa el viewport alrededor del jugador (en píxeles del mapa)
    void updateViewport(float playerMapX, float playerMapY);

    // Conversión world(server) -> minimap (usa viewport)
    int worldToMinimapX(float serverX) const;
    int worldToMinimapY(float serverY) const;

    void renderCheckpoints(SDL2pp::Renderer& renderer);
    void renderPlayer(SDL2pp::Renderer& renderer, const MinimapPlayer& p, bool isLocal);

public:
    explicit Minimap(int size);
    ~Minimap();

    Minimap(const Minimap&) = delete;
    Minimap& operator=(const Minimap&) = delete;
    Minimap(Minimap&&) = default;
    Minimap& operator=(Minimap&&) = default;

    void loadMapImage(SDL2pp::Renderer& renderer, const std::string& imagePath);

    // Configurar transformación servidor->mapa
    void setWorldScale(float sx, float sy) { scaleX = sx; scaleY = sy; }
    void setWorldOffset(float ox, float oy) { offsetX = ox; offsetY = oy; }

    // Ajustar zoom (dimensiones del viewport en píxeles del mapa)
    void setZoomPixels(float w, float h) {
        zoomPixelWidth  = std::max(50.0f, w);
        zoomPixelHeight = std::max(50.0f, h);
    }

    void setCheckpoints(const std::vector<RaceCheckpoint>& cp);

    void render(SDL2pp::Renderer& renderer,
                const MinimapPlayer& localPlayer,
                const std::vector<MinimapPlayer>& otherPlayers);
};

#endif