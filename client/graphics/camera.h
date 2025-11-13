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
    Camera(int screenW, int screenH, float zoomInicial = 1.5f);

    void follow(float objX, float objY);

    // Devuelve srcRect para usar en renderer.Copy()
    SDL_Rect getSrcRect(int worldW, int worldH);

    void setDimensions(int w, int h);
    void setZoom(float z);
};

#endif  // _CAMERA_H
