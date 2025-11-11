#ifndef COLLISION_EXPLOSION_H
#define COLLISION_EXPLOSION_H

#include <SDL2pp/SDL2pp.hh>
#include <vector>

struct Particle {
    float x, y;           // Position (in screen coords)
    float vx, vy;         // Velocity
    float lifetime;       // Current lifetime
    float maxLifetime;    // Max lifetime before death
    int size;             // Particle size
};

class CollisionExplosion {
public:
    CollisionExplosion();
    
    // Trigger explosion at world position, with camera/scale parameters for screen transformation
    void trigger(float worldX, float worldY, float camX, float camY, float scale);
    
    // Update particles
    void update(float dt);
    
    // Render particles
    void render(SDL2pp::Renderer& renderer);
    
    // Check if explosion is active
    bool isActive() const { return !particles.empty(); }
    
private:
    std::vector<Particle> particles;
    const float explosionDuration = 1.0f;      // Duration in seconds
    const int particleCount = 30;              // More particles for visible effect
    const float particleSpeed = 400.0f;        // pixels/second (increased from 200)
    
    // Generate random number between min and max
    float randomFloat(float min, float max);
};

#endif  // COLLISION_EXPLOSION_H