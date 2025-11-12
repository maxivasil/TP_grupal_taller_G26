#ifndef CAR_SOUND_ENGINE_H
#define CAR_SOUND_ENGINE_H

#include <SDL_mixer.h>
#include <unordered_map>
#include <memory>
#include <vector>

// Car sound states
enum class CarSoundState {
    IDLE,
    ACCELERATING,
    TURNING,
    BRAKING
};

class CarSoundEngine {
private:
    bool audioInitialized;
    Mix_Chunk* engineSound;
    Mix_Chunk* turnSound;
    Mix_Chunk* brakeSound;
    
    int currentChannel;
    CarSoundState currentState;
    float currentSpeed;
    float previousSpeed;
    
    // State tracking (like explosion damage detection)
    bool previousTurning;
    bool previousBraking;
    
    // Sound generation
    Mix_Chunk* createEngineSound(float pitch);
    Mix_Chunk* createTurnSound();
    Mix_Chunk* createBrakeSound();
    void initAudio();
    
public:
    CarSoundEngine();
    ~CarSoundEngine();
    
    // Update sound based on car state
    void update(float speed, bool isTurning, bool isBraking);
    
    // Trigger specific sounds
    void playAcceleration();
    void playTurnSound();
    void playBrakeSound();
    void stop();
};

#endif  // CAR_SOUND_ENGINE_H
