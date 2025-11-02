#ifndef TRACK_H
#define TRACK_H

#include <vector>
#include <string>
#include <SDL2/SDL.h>

struct Checkpoint {
    float x, y, width, height;
    int id;
    bool isFinish;
};

class Track {
private:
    std::string name;
    std::vector<Checkpoint> checkpoints;
    float startX, startY;
    
public:
    Track(const std::string& mapName);
    
    const std::vector<Checkpoint>& getCheckpoints() const { return checkpoints; }
    bool getStartPosition(float& x, float& y) const;
    
    // Detección de checkpoints
    int checkpointAtPosition(float carX, float carY, float carWidth, float carHeight) const;
    bool isCheckpointFinish(int checkpointId) const;
    
    void render(SDL_Renderer* renderer, const SDL_FRect& camera) const;
};

#endif
