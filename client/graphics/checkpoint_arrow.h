#ifndef CLIENT_CHECKPOINT_ARROW_H
#define CLIENT_CHECKPOINT_ARROW_H

#include <SDL2pp/SDL2pp.hh>
#include <cmath>

class CheckpointArrow {
private:
    int windowWidth;
    int windowHeight;
    
    int arrowX;
    int arrowY;
    int arrowSize = 40;
    
    float angleToCheckpoint = 0.0f;
    float distanceToCheckpoint = 0.0f;
    bool hasTarget = false;

public:
    explicit CheckpointArrow(int windowWidth, int windowHeight);
    ~CheckpointArrow() = default;
    
    CheckpointArrow(const CheckpointArrow&) = delete;
    CheckpointArrow& operator=(const CheckpointArrow&) = delete;
    CheckpointArrow(CheckpointArrow&&) = default;
    CheckpointArrow& operator=(CheckpointArrow&&) = default;
    
    void updateTarget(float playerX, float playerY,
                     float checkpointX, float checkpointY,
                     float playerHeading = 0.0f);
    
    void render(SDL2pp::Renderer& renderer);
    
    bool hasActiveTarget() const { return hasTarget; }
    float getDistance() const { return distanceToCheckpoint; }
    float getAngle() const { return angleToCheckpoint; }

private:
    void drawArrow(SDL2pp::Renderer& renderer, float centerX, float centerY, 
                  float angle, int size);
    
    void drawFilledTriangle(SDL2pp::Renderer& renderer,
                           float x1, float y1, float x2, float y2, 
                           float x3, float y3, SDL_Color color);
};

#endif
