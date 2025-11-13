#ifndef COLLISION_EXPLOSION_H
#define COLLISION_EXPLOSION_H

#include <SDL2pp/SDL2pp.hh>
#include <SDL_mixer.h>
#include <vector>

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
    
    void setSoundEngine(CarSoundEngine* engine) { soundEngine = engine; }
    
    void trigger(float worldX, float worldY, float camX, float camY, float scale);
    
    void update(float dt);
    
    void render(SDL2pp::Renderer& renderer);
    
    bool isActive() const { return !particles.empty(); }
    
private:
    std::vector<Particle> particles;
    const float explosionDuration = 1.0f;      // Duration in seconds
    const int particleCount = 30;              // More particles for visible effect
    const float particleSpeed = 400.0f;        // pixels/second (increased from 200)
    
    CarSoundEngine* soundEngine = nullptr;
    
    float randomFloat(float min, float max);
};

#endif  
