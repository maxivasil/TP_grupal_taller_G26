#ifndef CAR_FIRE_EFFECT_H
#define CAR_FIRE_EFFECT_H

#include <vector>

#include <SDL2pp/SDL2pp.hh>

class CarFireEffect {
public:
    CarFireEffect();
    ~CarFireEffect();

    void start(float screenX, float screenY, float carWidth, float carHeight);

    void update(float dt);

    void render(SDL2pp::Renderer& renderer);

    void stop();

private:
    struct FireParticle {
        float x, y;
        float vx, vy;
        float lifetime;
        float maxLifetime;
        float size;
        uint8_t r, g, b;  // Color del fuego
    };

    std::vector<FireParticle> particles;
    bool active = false;
    float baseX = 0.0f, baseY = 0.0f;
    float carWidth = 0.0f, carHeight = 0.0f;
    float emissionTimer = 0.0f;
    const float emissionInterval = 0.02f;  // Emitir particulas cada 20ms

    float randomFloat(float min, float max);
    uint8_t randomByte(uint8_t min, uint8_t max);

    /**
     * @brief Emite nuevas partículas de fuego
     */
    void emitFireParticles();
};

#endif
