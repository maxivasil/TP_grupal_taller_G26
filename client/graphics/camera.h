#ifndef _CAMERA_H
#define _CAMERA_H

#include <map>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

class Camera {

private:
    float x, y;
    float screenW, screenH;
    float zoom;

public:
    Camera(int screenW, int screenH, float initialZoom);

    void follow(float objX, float objY);

    SDL_Rect getSrcRect(int worldW, int worldH);

    void setDimensions(int w, int h);
};

#endif  // _CAMERA_H
