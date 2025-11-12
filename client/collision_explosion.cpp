#include "collision_explosion.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <cstring>

CollisionExplosion::CollisionExplosion() 
    : explosionSound(nullptr), audioInitialized(false) {
    // Audio will be initialized on first explosion (lazy init)
}

CollisionExplosion::~CollisionExplosion() {
    if (explosionSound) {
        Mix_FreeChunk(explosionSound);
    }
    if (audioInitialized) {
        Mix_CloseAudio();
    }
}

float CollisionExplosion::randomFloat(float min, float max) {
    return min + (max - min) * (float)rand() / RAND_MAX;
}

void CollisionExplosion::initAudio() {
    if (audioInitialized) return;
    
    // Initialize SDL_mixer
    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        std::cerr << "Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
        return;
    }
    
    std::cout << "[EXPLOSION AUDIO] SDL_mixer initialized" << std::endl;
    audioInitialized = true;
    
    // Create explosion sound
    explosionSound = createExplosionSound();
    if (!explosionSound) {
        std::cerr << "Failed to create explosion sound" << std::endl;
    } else {
        std::cout << "[EXPLOSION AUDIO] Sound created successfully" << std::endl;
    }
}

Mix_Chunk* CollisionExplosion::createExplosionSound() {
    // Create a low-frequency "boom" sound for explosion
    int sampleRate = 22050;
    int duration = 300;  // milliseconds - longer than collision sound
    int samples = (sampleRate * duration) / 1000;
    
    // Allocate audio buffer
    Uint8* buffer = new Uint8[samples * 2];  // 16-bit mono
    
    // Generate a complex explosion sound with pitch sweep
    int16_t* samples16 = (int16_t*)buffer;
    float startFrequency = 150.0f;   // Start low
    float endFrequency = 50.0f;      // End even lower
    float amplitude = 20000.0f;
    
    for (int i = 0; i < samples; ++i) {
        // Frequency sweep from high to low
        float progress = (float)i / samples;
        float frequency = startFrequency + (endFrequency - startFrequency) * progress;
        
        float phase = (2.0f * 3.14159f * frequency * i) / sampleRate;
        
        // Envelope: attack + decay
        float envelope;
        if (progress < 0.1f) {
            envelope = progress / 0.1f;  // Attack
        } else {
            envelope = 1.0f - (progress - 0.1f) / 0.9f;  // Decay
        }
        
        samples16[i] = (int16_t)(amplitude * std::sin(phase) * envelope);
    }
    
    // Create chunk
    Mix_Chunk* chunk = new Mix_Chunk();
    chunk->allocated = 1;
    chunk->abuf = buffer;
    chunk->alen = samples * 2;
    chunk->volume = MIX_MAX_VOLUME;
    
    return chunk;
}

void CollisionExplosion::trigger(float worldX, float worldY, float camX, float camY, float scale) {
    // Initialize audio on first explosion if not already done
    if (!audioInitialized) {
        initAudio();
    }
    
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
    
    // Play explosion sound
    if (explosionSound) {
        Mix_PlayChannel(-1, explosionSound, 0);
        std::cout << "[EXPLOSION SOUND] Played!" << std::endl;
    } else {
        std::cout << "[EXPLOSION SOUND] WARNING: No explosion sound available" << std::endl;
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
