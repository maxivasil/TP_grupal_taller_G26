#include "camera.h"

#define LIMITE_X 12
#define LIMITE_Y 20

Camera::Camera(int screenW, int screenH, float initialZoom):
        x(0), y(0), screenW(screenW), screenH(screenH), zoom(initialZoom) {}

void Camera::follow(float objX, float objY) {
    x = objX;
    y = objY;
}

SDL_Rect Camera::getSrcRect(int worldW, int worldH) {
    int viewW = int(screenW / zoom);
    int viewH = int(screenH / zoom);

    int srcX = int(x - viewW / 2);
    int srcY = int(y - viewH / 2);

    if (srcX < 0)
        srcX = 0;
    if (srcY < 0)
        srcY = 0;
    if (srcX + viewW > worldW)
        srcX = worldW - viewW;
    if (srcY + viewH > worldH)
        srcY = worldH - viewH;

    SDL_Rect src{srcX, srcY, viewW, viewH};
    return src;
}

void Camera::setDimensions(int w, int h) {
    screenW = w;
    screenH = h;
}
