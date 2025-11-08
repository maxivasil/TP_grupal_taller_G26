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
    float x, y, angle;  // Coordenadas del servidor (sin transformar)
    int playerId;
    float health;
    bool isLocal;
};

class Minimap {
private:
    int size;  // Tamaño del minimap en píxeles (150x150)
    int arrowPixelSize = 8;
    std::unique_ptr<SDL2pp::Texture> mapTexture;  // PNG del mapa escalado
    std::vector<RaceCheckpoint> checkpoints;

    // Dimensiones reales del mapa en píxeles (se toman del PNG al cargar)
    float mapWidth = 0.0f;
    float mapHeight = 0.0f;

    // Transformación de coordenadas del servidor -> píxeles del mapa
    // pixel = server * scale + offset
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float offsetX = 0.0f;
    float offsetY = 0.0f;

    // Conversión de coordenadas del servidor a píxeles del minimap (usa transform y dims de mapa)
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

    // Configurar cómo pasar de coordenadas del servidor a píxeles del mapa
    void setWorldScale(float sx, float sy) { scaleX = sx; scaleY = sy; }
    void setWorldOffset(float ox, float oy) { offsetX = ox; offsetY = oy; }

    void setCheckpoints(const std::vector<RaceCheckpoint>& cp);
    void render(SDL2pp::Renderer& renderer,
                const MinimapPlayer& localPlayer,
                const std::vector<MinimapPlayer>& otherPlayers);
};

#endif