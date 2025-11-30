#ifndef COLLISION_EXPLOSION_H
#define COLLISION_EXPLOSION_H

#include <vector>

#include <SDL2pp/SDL2pp.hh>
#include <SDL_mixer.h>

class CarSoundEngine;

struct Particle {
    float x, y;
    float vx, vy;
    float lifetime;
    float maxLifetime;
    int size;
};

class CollisionExplosion {
public:
    CollisionExplosion();
    ~CollisionExplosion();

    void trigger(float worldX, float worldY, float camX, float camY, float scale);

    void triggerFinalExplosion(float worldX, float worldY, float camX, float camY, float scale);

    void update(float dt);

    void render(SDL2pp::Renderer& renderer);

    void stop();

private:
    std::vector<Particle> particles;
    const float explosionDuration = 1.0f;       // Duration in seconds
    const int particleCount = 30;               // More particles for visible effect
    const float particleSpeed = 400.0f;         // pixels/second (increased from 200)
    const float finalExplosionDuration = 1.5f;  // Longer duration for final explosion
    const int finalParticleCount = 80;          // More particles for final explosion
    const float finalParticleSpeed = 600.0f;    // Faster particles for final explosion

    float randomFloat(float min, float max);
};

#endif
