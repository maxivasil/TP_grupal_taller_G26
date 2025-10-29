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
    float angle;     // Ángulo en grados (0° = hacia arriba)
    float speed;     // Velocidad actual
};

// === Funciones auxiliares ===

// Convierte grados a radianes
float deg2rad(float deg) {
    return deg * M_PI / 180.0f;
}

// === Programa principal ===
int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Minimapa fijo + flecha rotando",
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

        // Avanzar según el ángulo (SDL: Y crece hacia abajo, 0° apunta arriba)
        const float ang = deg2rad(car.angle);
        car.x += std::sin(ang) * car.speed;   // X hacia la derecha
        car.y -= std::cos(ang) * car.speed;   // Y hacia arriba (nota el '-')
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

        // Dibujar edificios (vista principal)
        SDL_SetRenderDrawColor(renderer, 80, 100, 255, 255);
        for (auto& b : buildings) {
            SDL_FRect screenRect = {
                b.rect.x - camera.x,
                b.rect.y - camera.y,
                b.rect.w, b.rect.h
            };
            SDL_RenderFillRectF(renderer, &screenRect);
        }

        // Auto como triángulo rotado (vista principal)
        float cx = SCREEN_WIDTH / 2.0f;
        float cy = SCREEN_HEIGHT / 2.0f;

        float halfWidth = 10.0f;
        float halfLength = 15.0f;

        float localPoints[3][2] = {
            {0.0f, -halfLength},        // Punta delantera
            {-halfWidth, halfLength},   // Trasera izquierda
            {halfWidth, halfLength}     // Trasera derecha
        };

        SDL_Vertex vertices[3];
        float angleRad = deg2rad(car.angle);
        float cosA = std::cos(angleRad);
        float sinA = std::sin(angleRad);

        for (int i = 0; i < 3; ++i) {
            float rotX = localPoints[i][0] * cosA - localPoints[i][1] * sinA;
            float rotY = localPoints[i][0] * sinA + localPoints[i][1] * cosA;

            vertices[i].position.x = cx + rotX;
            vertices[i].position.y = cy + rotY;
            vertices[i].color = SDL_Color{255, 60, 60, 255};  // Rojo
            vertices[i].tex_coord = SDL_FPoint{0, 0};
        }

        int indices[3] = {0, 1, 2};
        SDL_RenderGeometry(renderer, NULL, vertices, 3, indices, 3);

        // Punto amarillo en el frente (punta del triángulo) en la vista principal
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_FRect frontDot = {
            vertices[0].position.x - 3,
            vertices[0].position.y - 3,
            6, 6
        };
        SDL_RenderFillRectF(renderer, &frontDot);

        // ---- MINIMAPA (FIJO, NORTH-UP) ----
        SDL_Rect minimapViewport = { SCREEN_WIDTH - MINIMAP_SIZE - 20, 20, MINIMAP_SIZE, MINIMAP_SIZE };
        SDL_RenderSetViewport(renderer, &minimapViewport);

        // Fondo del minimapa
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderFillRect(renderer, NULL);

        // Escala de mundo -> minimapa (sin rotación)
        float scaleX = (float)MINIMAP_SIZE / WORLD_WIDTH;
        float scaleY = (float)MINIMAP_SIZE / WORLD_HEIGHT;

        // Edificios como puntos (no rotados)
        SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
        for (auto& b : buildings) {
            float bx = (b.rect.x - car.x) * scaleX + MINIMAP_SIZE / 2.0f;
            float by = (b.rect.y - car.y) * scaleY + MINIMAP_SIZE / 2.0f;

            // Pequeño rectángulo/píxel en el minimapa
            SDL_Rect miniRect = { (int)bx, (int)by, 3, 3 };
            SDL_RenderFillRect(renderer, &miniRect);
        }

        // Flecha del auto en el minimapa: triángulo centrado que SÍ rota con car.angle
        float mCx = MINIMAP_SIZE / 2.0f;
        float mCy = MINIMAP_SIZE / 2.0f;

        // Triángulo pequeño en coords locales (apunta "arriba" en reposo)
        float miniHalfW = 4.0f;
        float miniHalfL = 6.0f;

        float miniLocal[3][2] = {
            {0.0f, -miniHalfL},        // Punta (frente)
            {-miniHalfW, miniHalfL},   // Trasera izquierda
            {miniHalfW, miniHalfL}     // Trasera derecha
        };

        float mCos = std::cos(angleRad);
        float mSin = std::sin(angleRad);

        SDL_FPoint miniPts[3];
        for (int i = 0; i < 3; ++i) {
            float rx = miniLocal[i][0] * mCos - miniLocal[i][1] * mSin;
            float ry = miniLocal[i][0] * mSin + miniLocal[i][1] * mCos;
            miniPts[i].x = mCx + rx;
            miniPts[i].y = mCy + ry;
        }

        // Dibujar el triángulo del auto en el minimapa
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (int i = 0; i < 3; ++i) {
            int nex = (i + 1) % 3;
            SDL_RenderDrawLineF(renderer, miniPts[i].x, miniPts[i].y, miniPts[nex].x, miniPts[nex].y);
        }
        SDL_RenderDrawLineF(renderer, miniPts[2].x, miniPts[2].y, miniPts[0].x, miniPts[0].y);

        // Punto amarillo en la punta del triángulo del minimapa
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_FRect miniFrontDot = {
            miniPts[0].x - 2,
            miniPts[0].y - 2,
            4, 4
        };
        SDL_RenderFillRectF(renderer, &miniFrontDot);

        // Restablecer viewport y presentar
        SDL_RenderSetViewport(renderer, NULL);
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}