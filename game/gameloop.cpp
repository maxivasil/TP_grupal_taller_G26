#include "gameloop.h"
#include <iostream>

GameLoop::GameLoop()
    : window(nullptr),
      renderer(nullptr),
      running(true),
      world(),
      city(LibertyCity),  // ✅ Cargar Liberty City
      carStats{
    .acceleration = 12000.0f,     // ✅ Aumentar aceleración
    .max_speed = 100.0f,          // ✅ Aumentar velocidad máxima
    .turn_speed = 3.5f,           // ✅ Girar un poco más rápido
    .mass = 1500.0f,
    .brake_force = 6000.0f,       // ✅ Freno más potente
    .handling = 1.0f,
    .health_max = 100.0f,
    .length = 4.0f,
    .width = 2.0f
},
      car(world.getWorldId(), carStats, {400.0f, 300.0f}, b2MakeRot(0.0f)),
      minimap(200) {
    
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Taller Game - Liberty City",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

GameLoop::~GameLoop() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void GameLoop::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        }
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
            running = false;
        }
    }
}

void GameLoop::update(float deltaTime) {
    const Uint8* keys = SDL_GetKeyboardState(nullptr);
    
    CarInput input;
    input.accelerating = keys[SDL_SCANCODE_W];
    input.braking = keys[SDL_SCANCODE_S];
    
    if (keys[SDL_SCANCODE_A]) {
        input.turn_direction = Direction::LEFT;
    } else if (keys[SDL_SCANCODE_D]) {
        input.turn_direction = Direction::RIGHT;
    } else {
        input.turn_direction = Direction::NONE;
    }
    
    car.updatePhysics(input);
    b2World_Step(world.getWorldId(), deltaTime, 4);
    car.update(deltaTime);
}

void GameLoop::render() {
    // Fondo verde césped
    SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
    SDL_RenderClear(renderer);
    
    b2Vec2 carPos = car.getPosition();
    SDL_FRect camera = { carPos.x - 400, carPos.y - 300, 800, 600 };
    
    // ✅ 1. Renderizar el mapa de la ciudad
    city.render(renderer, camera);
    
    // ✅ 2. Renderizar el auto encima
    car.render(renderer, camera);
    
    // ✅ 3. Minimapa al final
    minimap.render(renderer, world, car);
    
    SDL_RenderPresent(renderer);
}

void GameLoop::run() {
    Uint64 last = SDL_GetPerformanceCounter();
    Uint64 now = 0;
    double deltaTime = 0;

    while (running) {
        now = SDL_GetPerformanceCounter();
        deltaTime = (double)(now - last) / (double)SDL_GetPerformanceFrequency();
        last = now;

        handleEvents();
        update((float)deltaTime);
        render();
        
        SDL_Delay(1);
    }
}
