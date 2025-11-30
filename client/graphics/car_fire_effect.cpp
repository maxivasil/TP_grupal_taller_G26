#include "car_fire_effect.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

CarFireEffect::CarFireEffect() {}

CarFireEffect::~CarFireEffect() {}

float CarFireEffect::randomFloat(float min, float max) {
    return min + (max - min) * (float)rand() / RAND_MAX;
}

uint8_t CarFireEffect::randomByte(uint8_t min, uint8_t max) {
    return min + (uint8_t)((max - min) * (float)rand() / RAND_MAX);
}

void CarFireEffect::start(float screenX, float screenY, float carWidth, float carHeight) {
    if (carWidth <= 0 || carHeight <= 0 || carWidth > 1000 || carHeight > 1000) {
        std::cerr << "[FIRE ERROR] Invalid car dimensions: " << carWidth << "x" << carHeight << std::endl;
        active = false;
        return;
    }

    active = true;
    baseX = screenX;
    baseY = screenY;
    this->carWidth = carWidth;
    this->carHeight = carHeight;
    particles.clear();
    emissionTimer = 0.0f;

    std::cout << "[FIRE] Fire effect started at screen (" << screenX << ", " << screenY
              << ") car size (" << carWidth << "x" << carHeight << ")" << std::endl;
}

void CarFireEffect::emitFireParticles() {
    int particleCount = 5 + rand() % 4;

    for (int i = 0; i < particleCount; ++i) {
        FireParticle p;

        float angle = randomFloat(0.0f, 2.0f * 3.14159f);
        float distance = randomFloat(carWidth * 0.15f, carWidth * 0.3f);  // Más cercano
        
        p.x = baseX + std::cos(angle) * distance;
        p.y = baseY + std::sin(angle) * distance * 0.7f;

        float speed = randomFloat(120.0f, 250.0f);
        p.vx = std::cos(angle) * speed;
        p.vy = std::sin(angle) * speed * 0.6f - 30.0f;  // Algo de ascenso también

        p.lifetime = 0.0f;
        p.maxLifetime = randomFloat(0.8f, 1.5f);

        p.size = randomFloat(8.0f, 18.0f);

        int colorType = rand() % 3;
        if (colorType == 0) {  // Naranja
            p.r = 255;
            p.g = randomByte(140, 180);
            p.b = 0;
        } else if (colorType == 1) {  // Rojo
            p.r = 255;
            p.g = randomByte(0, 80);
            p.b = 0;
        } else {  // Amarillo
            p.r = 255;
            p.g = 200;
            p.b = 0;
        }

        particles.push_back(p);
    }
}

void CarFireEffect::update(float dt) {
    if (!active)
        return;

    emissionTimer += dt;
    while (emissionTimer >= emissionInterval) {
        emitFireParticles();
        emissionTimer -= emissionInterval;
    }

    for (auto it = particles.begin(); it != particles.end();) {
        it->lifetime += dt;

        it->x += it->vx * dt;
        it->y += it->vy * dt;

        it->vx *= 0.95f;

        it->vy -= 50.0f * dt;

        it->size *= 0.95f;

        if (it->lifetime >= it->maxLifetime || it->size < 1.0f) {
            it = particles.erase(it);
        } else {
            ++it;
        }
    }

    if (particles.empty() && emissionTimer > 0.5f) {
        active = false;
    }
}

void CarFireEffect::render(SDL2pp::Renderer& renderer) {
    if (!active || particles.empty())
        return;

    Uint8 r, g, b, a;
    renderer.GetDrawColor(r, g, b, a);

    for (auto& p : particles) {
        float progress = p.lifetime / p.maxLifetime;

        uint8_t alpha = static_cast<uint8_t>(255 * (1.0f - progress * 0.8f));

        uint8_t finalR = static_cast<uint8_t>(p.r * (1.0f - progress * 0.3f));
        uint8_t finalG = static_cast<uint8_t>(p.g * (1.0f - progress * 0.5f));
        uint8_t finalB = static_cast<uint8_t>(p.b * (1.0f - progress * 0.3f));

        renderer.SetDrawColor(finalR, finalG, finalB, alpha);
        renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);

        SDL_Rect rect = {static_cast<int>(p.x - p.size / 2), static_cast<int>(p.y - p.size / 2),
                         static_cast<int>(p.size), static_cast<int>(p.size)};
        renderer.FillRect(rect);

        renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
    }

    renderer.SetDrawColor(r, g, b, a);
}
