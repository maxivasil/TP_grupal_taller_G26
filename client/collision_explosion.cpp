#include "collision_explosion.h"
#include <cmath>
#include <cstdlib>
#include <iostream>

CollisionExplosion::CollisionExplosion() {}

float CollisionExplosion::randomFloat(float min, float max) {
    return min + (max - min) * (float)rand() / RAND_MAX;
}

void CollisionExplosion::trigger(float worldX, float worldY, float camX, float camY, float scale) {
    particles.clear();
    
    // Transform world coordinates to screen coordinates using camera and scale
    float relX = (worldX - camX) * scale;
    float relY = (worldY - camY) * scale;
    
    // Create explosion particles
    for (int i = 0; i < particleCount; ++i) {
        Particle p;
        p.x = relX;
        p.y = relY;
        
        // Random direction (360 degrees)
        float angle = randomFloat(0.0f, 2.0f * 3.14159f);
        float speed = randomFloat(particleSpeed * 0.5f, particleSpeed);
        
        p.vx = std::cos(angle) * speed;
        p.vy = std::sin(angle) * speed;
        
        p.lifetime = 0.0f;
        p.maxLifetime = randomFloat(explosionDuration * 0.7f, explosionDuration);
        p.size = randomFloat(6, 14);  // Bigger particles
        
        particles.push_back(p);
    }
    
    std::cout << "[EXPLOSION] Triggered at screen (" << relX << ", " << relY << ") with " 
              << particles.size() << " particles" << std::endl;
}

void CollisionExplosion::update(float dt) {
    for (auto it = particles.begin(); it != particles.end(); ) {
        it->lifetime += dt;
        
        // Update position with velocity and gravity
        it->x += it->vx * dt;
        it->y += it->vy * dt;
        
        // Apply gravity
        it->vy += 150.0f * dt;  // pixels/second^2
        
        // Remove dead particles
        if (it->lifetime >= it->maxLifetime) {
            it = particles.erase(it);
        } else {
            ++it;
        }
    }
}

void CollisionExplosion::render(SDL2pp::Renderer& renderer) {
    for (auto& p : particles) {
        // Calculate alpha based on lifetime
        float progress = p.lifetime / p.maxLifetime;
        uint8_t alpha = static_cast<uint8_t>(255 * (1.0f - progress));
        
        // Color gradient: yellow -> orange -> red as time progresses
        uint8_t red = 255;
        uint8_t green = static_cast<uint8_t>(255 * (1.0f - progress * 0.8f));
        uint8_t blue = static_cast<uint8_t>(50 * (1.0f - progress));
        
        renderer.SetDrawColor(red, green, blue, alpha);
        renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
        
        // Particles already in screen coordinates, no transformation needed
        SDL_Rect rect = {
            static_cast<int>(p.x - p.size / 2),
            static_cast<int>(p.y - p.size / 2),
            p.size,
            p.size
        };
        renderer.FillRect(rect);
        
        renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
    }
}
