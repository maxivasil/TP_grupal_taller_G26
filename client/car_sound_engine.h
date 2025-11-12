#ifndef CAR_SOUND_ENGINE_H
#define CAR_SOUND_ENGINE_H

#include <SDL_mixer.h>
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>

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
    
    int engineChannel = -1;
    int turnChannel = -1;
    int brakeChannel = -1;
    
    // Track if sounds are currently playing
    bool enginePlaying = false;
    bool turnPlaying = false;
    bool brakePlaying = false;
    
    // Sound loading
    Mix_Chunk* loadSound(const std::string& filepath);
    void initAudio();
    
    // Fallback: procedural sound generation if files not found
    Mix_Chunk* createEngineSound(float pitch);
    Mix_Chunk* createTurnSound();
    Mix_Chunk* createBrakeSound();
    
public:
    CarSoundEngine();
    ~CarSoundEngine();
    
    // Update sound based on what keys are currently pressed
    // Pass true/false for each action currently happening
    void update(bool isAccelerating, bool isTurning, bool isBraking);
    
    // Trigger specific sounds
    void playAcceleration();
    void playTurnSound();
    void playBrakeSound();
    void stopAcceleration();
    void stopTurn();
    void stopBrake();
    void stopAll();
};

#endif  // CAR_SOUND_ENGINE_H
