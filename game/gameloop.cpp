#include "gameloop.h"

GameLoop::GameLoop()
    : window(nullptr), renderer(nullptr), running(true),
      world(), car(world.getWorld()), minimap(200) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("TP - Need for Speed",
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
        if (e.type == SDL_QUIT)
            running = false;
    }
}

void GameLoop::update(float deltaTime) {
    car.update(deltaTime);
}

void GameLoop::render() {
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderClear(renderer);
    b2Vec2 carPos = car.getPosition();
    SDL_FRect camera = { carPos.x - 400, carPos.y - 300, 800, 600 };
    world.render(renderer, camera);
    car.render(renderer, camera);
    minimap.render(renderer, world, car);

    SDL_RenderPresent(renderer);
}

void GameLoop::run() {
    Uint64 now = SDL_GetPerformanceCounter(), last = 0;
    double deltaTime = 0;

    while (running) {
        last = now;
        now = SDL_GetPerformanceCounter();
        deltaTime = (double)((now - last) * 1000 / (double)SDL_GetPerformanceFrequency()) / 1000.0;

        handleEvents();
        update((float)deltaTime);
        render();
    }
}
