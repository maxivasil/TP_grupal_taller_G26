#include <SDL2/SDL.h>
#include <vector>
#include <cmath>
#include <algorithm>

// === Configuración general ===
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int WORLD_WIDTH = 2000;
const int WORLD_HEIGHT = 2000;

const int MINIMAP_SIZE = 200;

struct Building {
    SDL_FRect rect; // Coordenadas en el mundo
};

struct Car {
    float x, y;      // Posición en el mundo
    float angle;     // Ángulo en grados
    float speed;     // Velocidad actual
};

// === Funciones auxiliares ===

// Convierte grados a radianes
float deg2rad(float deg) {
    return deg * M_PI / 180.0f;
}

// Aplica rotación alrededor del centro del minimapa
void rotatePoint(float& x, float& y, float cx, float cy, float angleDeg) {
    float rad = deg2rad(angleDeg);
    float s = sin(rad);
    float c = cos(rad);
    x -= cx;
    y -= cy;
    float xnew = x * c - y * s;
    float ynew = x * s + y * c;
    x = xnew + cx;
    y = ynew + cy;
}

// === Programa principal ===
int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Minimapa Rotante",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Mundo: edificios
    std::vector<Building> buildings;
    for (int i = 0; i < 40; ++i) {
        buildings.push_back({ (float)(rand() % WORLD_WIDTH), (float)(rand() % WORLD_HEIGHT), 80, 80 });
    }

    // Auto del jugador
    Car car = { WORLD_WIDTH / 2.0f, WORLD_HEIGHT / 2.0f, 0.0f, 0.0f };
    const float maxSpeed = 8.0f;
    const float accel = 0.4f;
    const float decel = 0.2f;
    const float turnSpeed = 4.0f;

    bool running = true;
    SDL_Event e;

    while (running) {
        // ---- INPUT ----
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }
        const Uint8* state = SDL_GetKeyboardState(NULL);

        // Movimiento y rotación del auto
        if (state[SDL_SCANCODE_W])
            car.speed = std::min(car.speed + accel, maxSpeed);
        else if (state[SDL_SCANCODE_S])
            car.speed = std::max(car.speed - accel, -maxSpeed / 2);
        else
            car.speed *= (1 - decel * 0.1f); // fricción leve

        if (state[SDL_SCANCODE_A])
            car.angle -= turnSpeed * (car.speed / maxSpeed);
        if (state[SDL_SCANCODE_D])
            car.angle += turnSpeed * (car.speed / maxSpeed);

        // Avanzar según el ángulo
        car.x += cos(deg2rad(car.angle)) * car.speed;
        car.y += sin(deg2rad(car.angle)) * car.speed;

        // Limitar dentro del mundo
        car.x = std::clamp(car.x, 0.0f, (float)(WORLD_WIDTH));
        car.y = std::clamp(car.y, 0.0f, (float)(WORLD_HEIGHT));

        // ---- RENDER MUNDO ----
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_RenderClear(renderer);

        // Cámara centrada en el auto
        SDL_FRect camera = {
            car.x - SCREEN_WIDTH / 2.0f,
            car.y - SCREEN_HEIGHT / 2.0f,
            (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT
        };

        // Dibujar edificios
        SDL_SetRenderDrawColor(renderer, 80, 100, 255, 255);
        for (auto& b : buildings) {
            SDL_FRect screenRect = {
                b.rect.x - camera.x,
                b.rect.y - camera.y,
                b.rect.w, b.rect.h
            };
            SDL_RenderFillRectF(renderer, &screenRect);
        }

        // Auto (centro de pantalla)
        SDL_SetRenderDrawColor(renderer, 255, 60, 60, 255);
        SDL_FRect carRect = { SCREEN_WIDTH / 2.0f - 10, SCREEN_HEIGHT / 2.0f - 10, 20, 20 };
        SDL_RenderFillRectF(renderer, &carRect);

        // ---- MINIMAPA ----
        SDL_Rect minimapViewport = { SCREEN_WIDTH - MINIMAP_SIZE - 20, 20, MINIMAP_SIZE, MINIMAP_SIZE };
        SDL_RenderSetViewport(renderer, &minimapViewport);

        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderFillRect(renderer, NULL);

        float scaleX = (float)MINIMAP_SIZE / WORLD_WIDTH;
        float scaleY = (float)MINIMAP_SIZE / WORLD_HEIGHT;

        // Dibujar edificios rotados
        SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
        for (auto& b : buildings) {
            float bx = (b.rect.x - car.x) * scaleX + MINIMAP_SIZE / 2;
            float by = (b.rect.y - car.y) * scaleY + MINIMAP_SIZE / 2;
            rotatePoint(bx, by, MINIMAP_SIZE / 2, MINIMAP_SIZE / 2, -car.angle);
            SDL_Rect miniRect = { (int)bx, (int)by, 3, 3 };
            SDL_RenderFillRect(renderer, &miniRect);
        }

        // Auto centrado
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect miniCar = { MINIMAP_SIZE / 2 - 3, MINIMAP_SIZE / 2 - 3, 6, 6 };
        SDL_RenderFillRect(renderer, &miniCar);

        SDL_RenderSetViewport(renderer, NULL);
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
