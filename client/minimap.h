#ifndef CLIENT_MINIMAP_H
#define CLIENT_MINIMAP_H

#include <SDL2pp/SDL2pp.hh>
#include <vector>
#include <string>
#include <yaml-cpp/yaml.h>
#include <cmath>

struct Building {
    float x, y, width, height;
    bool isWater;
};

struct RaceCheckpoint {
    int id;
    float x, y, width, height;
    bool isFinish;
};

struct MinimapPlayer {
    float x, y, angle;
    int playerId;
    float health;
    bool isLocal;
};

class Minimap {
private:
    int size;
    std::vector<Building> buildings;
    std::vector<RaceCheckpoint> checkpoints;
    
    float worldWidth = 700.0f;
    float worldHeight = 600.0f;
    
   
    float zoomWidth = 100.0f;
    float zoomHeight = 100.0f;
    
    void renderBuilding(SDL2pp::Renderer& renderer, const Building& b,
                       float playerX, float playerY);
    void renderCheckpoint(SDL2pp::Renderer& renderer, const RaceCheckpoint& cp,
                         float playerX, float playerY);
    void renderPlayer(SDL2pp::Renderer& renderer, const MinimapPlayer& p,
                     float playerX, float playerY, bool isLocal);
    
    float worldToMinimapX(float worldX, float playerX) const;
    float worldToMinimapY(float worldY, float playerY) const;

public:
    explicit Minimap(int size);
    ~Minimap();
    
    Minimap(const Minimap&) = delete;
    Minimap& operator=(const Minimap&) = delete;
    Minimap(Minimap&&) = default;
    Minimap& operator=(Minimap&&) = default;
    
    void loadCityData(const std::string& yamlPath);
    void setCheckpoints(const std::vector<RaceCheckpoint>& cp);
    void render(SDL2pp::Renderer& renderer,
               const MinimapPlayer& localPlayer,
               const std::vector<MinimapPlayer>& otherPlayers);
};

#endif
