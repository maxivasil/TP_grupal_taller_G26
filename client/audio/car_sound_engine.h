#ifndef CAR_SOUND_ENGINE_H
#define CAR_SOUND_ENGINE_H

#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL_mixer.h>

// Car sound states
enum class CarSoundState { IDLE, ACCELERATING, TURNING, BRAKING };

class CarSoundEngine {
private:
    static CarSoundEngine* instance;
    bool audioInitialized;
    Mix_Chunk* engineAccelerationSound = nullptr;
    Mix_Chunk* engineLoopSound = nullptr;
    Mix_Chunk* turnSound;
    Mix_Chunk* brakeSound;
    Mix_Chunk* collisionSound;
    Mix_Chunk* engineNoiseSound;     // Background engine noise (constant)
    Mix_Chunk* raceFinishSound;      // Sound when race finishes (victory)
    Mix_Chunk* gameOverSound;        // Sound when player loses
    Mix_Chunk* cheatActivatedSound;  // Sound when cheat is activated
    Mix_Chunk* brakeLoudSound;       // Loud brake screech
    Mix_Chunk* otherCarCollision;    // Collision from other cars

    int engineChannel = -1;
    int engineStartChannel = -1;
    int engineLoopChannel = -1;
    int turnChannel = -1;
    int brakeChannel = -1;
    int collisionChannel = -1;
    int engineNoiseChannel = -1;   // For background engine noise
    int contextSoundChannel = -1;  // For contextual sounds (race finish, etc)
    int otherCarChannel = -1;      // For other car sounds

    // Track if sounds are currently playing
    bool enginePlaying = false;
    bool turnPlaying = false;
    bool brakePlaying = false;
    bool engineNoisePlaying = false;

    // Distance-based sound throttling
    Uint32 lastCollisionSoundTime = 0;
    Uint32 lastBrakeSoundTime = 0;
    const Uint32 COLLISION_SOUND_COOLDOWN_MS = 100;  // Avoid sound spam
    const Uint32 BRAKE_SOUND_COOLDOWN_MS = 150;

    // Sound loading
    Mix_Chunk* loadSound(const std::string& filepath);
    void initAudio();

    // Fallback: procedural sound generation if files not found
    Mix_Chunk* createEngineSound(float pitch);
    Mix_Chunk* createTurnSound();
    Mix_Chunk* createBrakeSound();
    Mix_Chunk* createCollisionSound();
    Mix_Chunk* createVictorySound();  // Victory/finish sound

    static void onChannelFinished(int channel);

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
    void playCollisionSound();
    void playEngineNoise();  // Start background engine noise (looped)
    void stopAcceleration();
    void stopTurn();
    void stopBrake();
    void stopEngineNoise();
    void stopAll();

    // Contextual sounds with distance modulation
    void playRaceFinish();      // When player finishes race (victory)
    void playGameOver();        // When player loses
    void playCheatActivated();  // When cheat is activated
    void playOtherCarCollision(float distance, float intensity);  // Other cars colliding nearby
    void playDistantBrake(float distance);  // Other cars braking with distance-based volume

    // Distance-based volume calculation
    // maxDistance: sounds beyond this distance are silent (0.0)
    static float calculateDistanceVolume(float distance, float maxDistance = 100.0f);

    // Audio mute/unmute control
    void toggleAudioMute();
    void setAudioMute(bool muted);
    bool isAudioMuted() const { return audioMuted; }

private:
    bool audioMuted = false;  // Track mute state
};

#endif  // CAR_SOUND_ENGINE_H
