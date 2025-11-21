#include "checkpoint_arrow.h"

#include <iostream>

CheckpointArrow::CheckpointArrow(int windowWidth, int windowHeight):
        windowWidth(windowWidth),
        windowHeight(windowHeight),
        arrowX(windowWidth / 2),
        arrowY(60),
        arrowSize(40) {}

void CheckpointArrow::updateTarget(float playerX, float playerY, float checkpointX,
                                   float checkpointY, float playerHeading) {
    float dx = checkpointX - playerX;
    float dy = checkpointY - playerY;

    distanceToCheckpoint = std::sqrt(dx * dx + dy * dy);
    hasTarget = distanceToCheckpoint > 0.0f;

    if (hasTarget) {
        angleToCheckpoint = std::atan2(dx, -dy);
    }
}

void CheckpointArrow::drawArrow(SDL2pp::Renderer& renderer, float centerX, float centerY,
                                float angle, int size) {
    Uint8 r, g, b, a;
    renderer.GetDrawColor(r, g, b, a);

    float cos_a = std::cos(angle);
    float sin_a = std::sin(angle);


    float tipX = centerX + sin_a * size;
    float tipY = centerY - cos_a * size;

    float baseX = centerX - sin_a * size;
    float baseY = centerY + cos_a * size;

    float arrowLen = size * 0.4f;
    float arrowWidth = size * 0.3f;

    float headUpperX = tipX - sin_a * arrowLen - cos_a * arrowWidth;
    float headUpperY = tipY + cos_a * arrowLen - sin_a * arrowWidth;

    float headLowerX = tipX - sin_a * arrowLen + cos_a * arrowWidth;
    float headLowerY = tipY + cos_a * arrowLen + sin_a * arrowWidth;

    renderer.SetDrawColor(255, 200, 0, 255);
    renderer.DrawLine(SDL2pp::Point(int(baseX), int(baseY)), SDL2pp::Point(int(tipX), int(tipY)));

    renderer.DrawLine(SDL2pp::Point(int(tipX), int(tipY)),
                      SDL2pp::Point(int(headUpperX), int(headUpperY)));
    renderer.DrawLine(SDL2pp::Point(int(tipX), int(tipY)),
                      SDL2pp::Point(int(headLowerX), int(headLowerY)));

    for (int offset = -1; offset <= 1; offset++) {
        float perpX = -cos_a * offset * 1.5f;
        float perpY = -sin_a * offset * 1.5f;

        if (offset == 0)
            continue;

        renderer.SetDrawColor(255, 200, 0, 120);
        renderer.DrawLine(SDL2pp::Point(int(baseX + perpX), int(baseY + perpY)),
                          SDL2pp::Point(int(tipX + perpX), int(tipY + perpY)));
    }
    renderer.SetDrawColor(r, g, b, a);
}

void CheckpointArrow::drawFilledTriangle(SDL2pp::Renderer& /* renderer */, float /* x1 */,
                                         float /* y1 */, float /* x2 */, float /* y2 */,
                                         float /* x3 */, float /* y3 */, SDL_Color /* color */) {}

void CheckpointArrow::render(SDL2pp::Renderer& renderer) {
    if (!hasTarget) {
        return;
    }
    Uint8 r, g, b, a;
    renderer.GetDrawColor(r, g, b, a);

    renderer.SetDrawColor(50, 50, 50, 120);
    SDL_Rect bgCircle = {arrowX - arrowSize - 5, arrowY - arrowSize - 5, (arrowSize + 5) * 2,
                         (arrowSize + 5) * 2};
    renderer.FillRect(bgCircle);

    renderer.SetDrawColor(255, 200, 0, 180);
    for (int i = 0; i < 2; i++) {
        SDL_Rect border = {arrowX - (arrowSize + 5 + i), arrowY - (arrowSize + 5 + i),
                           (arrowSize + 5 + i) * 2, (arrowSize + 5 + i) * 2};
        renderer.DrawRect(border);
    }

    drawArrow(renderer, arrowX, arrowY, angleToCheckpoint, arrowSize);

    renderer.SetDrawColor(r, g, b, a);
}

void CheckpointArrow::onWindowResize(int w, int h, float scale) {
    windowWidth = w;
    windowHeight = h;
    arrowSize = int(40 * scale);

    arrowX = windowWidth / 2;
    const int marginTop = int(10 * scale);
    const int borderPad = int(5 * scale);

    arrowY = marginTop + arrowSize + borderPad;
}
