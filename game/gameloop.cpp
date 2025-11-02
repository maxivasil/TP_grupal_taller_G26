#include "gameloop.h"
#include <iostream>

GameLoop::GameLoop()
    : window(nullptr),
      renderer(nullptr),
      running(true),
      world(),
      map(5000, 5000, 42),
      city(LibertyCity),
      track("liberty_city"),  // ✅ Cargar track
      player(),  // ✅ Crear jugador
      currentCity(LibertyCity),
      carStats{
          .acceleration = 12000.0f,
          .max_speed = 100.0f,
          .turn_speed = 3.5f,
          .mass = 1500.0f,
          .brake_force = 6000.0f,
          .handling = 1.0f,
          .health_max = 100.0f,
          .length = 4.0f,
          .width = 2.0f
      },
      car(world.getWorldId(), carStats, {400.0f, 300.0f}, b2MakeRot(0.0f)),
      minimap(300) {  // ✅ Aumentar minimap a 300
    
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Taller Game - Liberty City",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    map.addToPhysicsWorld(world.getWorldId());
    
    // ✅ Inicializar player con cantidad de checkpoints
    player.setTotalCheckpoints(track.getCheckpoints().size());
    
    // ✅ Posicionar auto en el inicio del track
    float startX, startY;
    track.getStartPosition(startX, startY);
    // Aquí habría que reiniciar el auto en startX, startY
}

GameLoop::~GameLoop() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// ✅ Nuevo método para cambiar ciudad
void GameLoop::changeCity(CityName newCity) {
    if (currentCity == newCity) return;
    
    currentCity = newCity;
    city = City(newCity);
    
    // Cargar track de la nueva ciudad
    std::string cityNames[] = {"liberty_city", "san_andreas", "vice_city"};
    track = Track(cityNames[newCity]);
    
    // Reiniciar player
    player.reset();
    player.setTotalCheckpoints(track.getCheckpoints().size());
    
    // Cambiar título de la ventana
    std::string titles[] = {"Liberty City", "San Andreas", "Vice City"};
    std::string title = std::string("Taller Game - ") + titles[newCity];
    SDL_SetWindowTitle(window, title.c_str());
    
    std::cout << "Changed city to " << titles[newCity] << std::endl;
}

void GameLoop::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        }
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_1:  // ✅ Tecla 1 = Liberty City
                    changeCity(LibertyCity);
                    break;
                case SDLK_2:  // ✅ Tecla 2 = San Andreas
                    changeCity(SanAndreas);
                    break;
                case SDLK_3:  // ✅ Tecla 3 = Vice City
                    changeCity(ViceCity);
                    break;
                default:
                    break;
            }
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
    
    // ✅ Detectar checkpoints
    b2Vec2 carPos = car.getPosition();
    int checkpointId = track.checkpointAtPosition(
        carPos.x, carPos.y,
        carStats.width, carStats.length
    );
    
    if (checkpointId >= 0) {
        if (!player.hasCompletedCheckpoint(checkpointId)) {
            player.completeCheckpoint(checkpointId);
            
            // Si es el final, terminar la carrera
            if (track.isCheckpointFinish(checkpointId)) {
                player.finishRace();
            }
        }
    }
}

void GameLoop::render() {
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderClear(renderer);
    
    b2Vec2 carPos = car.getPosition();
    SDL_FRect camera = { carPos.x - 400, carPos.y - 300, 800, 600 };
    
    map.render(renderer, camera);
    car.render(renderer, camera);
    
    // ✅ Renderizar track (checkpoints)
    track.render(renderer, camera);
    
    minimap.render(renderer, world, car, city);
    
    // ✅ Mostrar estadísticas del jugador en pantalla
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    // Nota: Aquí necesitarías una librería de fuentes (SDL_ttf) para texto
    // Por ahora solo imprimimos en consola
    if (player.getCheckpointsCompleted() % 10 == 0 && player.getCheckpointsCompleted() > 0) {
        // Parpadea cada 10 checkpoints (efecto visual temporal)
    }
    
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
