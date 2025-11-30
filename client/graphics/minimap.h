#ifndef CLIENT_MINIMAP_H
#define CLIENT_MINIMAP_H

#include <algorithm>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include <SDL2pp/SDL2pp.hh>

struct RaceCheckpoint {
    int id;
    float x, y, width, height;
    bool isFinish;
};

struct MinimapPlayer {
    float x, y, angle;
};

class Minimap {
private:
    int size;
    int arrowPixelSize;

    std::unique_ptr<SDL2pp::Texture> mapTexture;

    std::vector<RaceCheckpoint> checkpoints;

    float mapWidth;
    float mapHeight;

    float scaleX;
    float scaleY;
    float offsetX;
    float offsetY;

    float zoomPixelWidth;
    float zoomPixelHeight;
    float viewLeft;
    float viewTop;

    void updateViewport(float playerMapX, float playerMapY);

    int worldToMinimapX(float serverX) const;
    int worldToMinimapY(float serverY) const;

    void renderCheckpoints(SDL2pp::Renderer& renderer, int nextCheckpointId);
    void renderPlayer(SDL2pp::Renderer& renderer, const MinimapPlayer& p);

public:
    explicit Minimap(int size);
    ~Minimap();

    Minimap(const Minimap&) = delete;
    Minimap& operator=(const Minimap&) = delete;
    Minimap(Minimap&&) = default;
    Minimap& operator=(Minimap&&) = default;

    void loadMapImage(SDL2pp::Renderer& renderer, const std::string& imagePath);

    void setWorldScale(float sx, float sy);

    void setZoomPixels(float w, float h);

    void setCheckpoints(const std::vector<RaceCheckpoint>& cp);

    void render(SDL2pp::Renderer& renderer, const MinimapPlayer& localPlayer, int nextCheckpointId);
    void onWindowResize(int w, int h, float scale);
};

#endif
