#ifndef GAMELOOP_H
#define GAMELOOP_H

#include <SDL2/SDL.h>
#include "world.h"
#include "car.h"
#include "minimap_renderer.h"
#include "procedural_map.h"
#include "city.h"
#include "track.h"
#include "player.h"

/**
 * @class GameLoop
 * @brief Controla el flujo principal del juego: eventos, actualización y renderizado.
 */
class GameLoop {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    
    World world;
    ProceduralMap map;
    City city;
    Track track;
    Player player;
    CityName currentCity;  // ✅ Agregar
    
    CarStats carStats{
    .acceleration = 5000.0f,      // ← Aumentado de 50
    .max_speed = 3000.0f,           // ← Reducido (velocidad en m/s de Box2D)
    .turn_speed = 3.0f,           // ← Ajustado
    .mass = 1500.0f,
    .brake_force = 3000.0f,       // ← Aumentado de 80
    .handling = 1.0f,
    .health_max = 100.0f,
    .length = 4.0f,
    .width = 2.0f
    };
    Car car;
    MinimapRenderer minimap;
    
    void handleEvents();
    void update(float deltaTime);
    void render();
    void changeCity(CityName newCity);  // ✅ Agregar método

public:
    GameLoop();
    ~GameLoop();
    void run();
};

#endif
