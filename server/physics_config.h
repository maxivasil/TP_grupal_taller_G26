#ifndef PHYSICS_CONFIG_H
#define PHYSICS_CONFIG_H

namespace PhysicsConfig {
    // Simulation timing
    constexpr float TIME_STEP = 1.0f / 60.0f;
    constexpr int VELOCITY_ITERATIONS = 8;
    constexpr int POSITION_ITERATIONS = 3;
    
    // Car physics tuning
    constexpr float DEFAULT_LINEAR_DAMPING = 0.3f;   // Air resistance
    constexpr float DEFAULT_ANGULAR_DAMPING = 0.5f;  // Rotation slowdown
    constexpr float DRIFT_FRICTION_MULTIPLIER = 0.6f;
    
    // Collision parameters
    constexpr float MIN_DAMAGE_IMPULSE = 0.001f;
    constexpr float DAMAGE_SCALING = 100000.0f;
    
    // Terrain types
    enum class TerrainType {
        TARMAC,    // Normal road - full grip
        GRASS,     // Slippery - reduced grip  
        GRAVEL,    // Very slippery
        MUD        // Extremely slippery + slowdown
    };
    
    struct TerrainProperties {
        float friction;
        float speedMultiplier;
        float damping;
    };
    
    inline TerrainProperties getTerrainProperties(TerrainType type) {
        switch(type) {
            case TerrainType::TARMAC:
                return {0.8f, 1.0f, 0.1f};
            case TerrainType::GRASS:
                return {0.4f, 0.85f, 0.3f};
            case TerrainType::GRAVEL:
                return {0.3f, 0.7f, 0.4f};
            case TerrainType::MUD:
                return {0.2f, 0.5f, 0.6f};
            default:
                return {0.8f, 1.0f, 0.1f};
        }
    }
}

#endif