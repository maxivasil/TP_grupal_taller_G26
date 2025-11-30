#ifndef CLIENT_CHECKPOINT_ARROW_H
#define CLIENT_CHECKPOINT_ARROW_H

#include <cmath>

#include <SDL2pp/SDL2pp.hh>

class CheckpointArrow {
private:
    int windowWidth;
    int windowHeight;

    int arrowX;
    int arrowY;
    int arrowSize;

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

    void updateTarget(float playerX, float playerY, float checkpointX, float checkpointY,
                      float playerHeading = 0.0f);

    void render(SDL2pp::Renderer& renderer);

    void onWindowResize(int w, int h, float scale);

private:
    void drawArrow(SDL2pp::Renderer& renderer, float centerX, float centerY, float angle, int size);
};

#endif
