#ifndef CAR_SOUND_ENGINE_H
#define CAR_SOUND_ENGINE_H

#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <SDL_mixer.h>

// Car sound states
enum class CarSoundState { IDLE, ACCELERATING, TURNING, BRAKING };

class CarSoundEngine {
private:
    static CarSoundEngine* instance;
    bool audioInitialized;
    Mix_Chunk* engineAccelerationSound;
    Mix_Chunk* engineLoopSound;
    Mix_Chunk* turnSound;
    Mix_Chunk* brakeSound;
    Mix_Chunk* collisionSound;
    Mix_Chunk* raceFinishSound;      // Sound when race finishes (victory)
    Mix_Chunk* gameOverSound;        // Sound when player loses
    Mix_Chunk* cheatActivatedSound;  // Sound when cheat is activated
    Mix_Chunk* championshipWinSound; // Sound when championship is won
    Mix_Chunk* countdownRaceSound;   // Sound for race countdown
    Mix_Music* musicTrack;

    int engineStartChannel = -1;
    int engineLoopChannel = -1;
    int turnChannel = -1;
    int brakeChannel = -1;
    int collisionChannel = -1;
    int contextSoundChannel = -1;  // For contextual sounds (race finish, etc)
    int otherCarChannel = -1;      // For other car sounds

    // Track if sounds are currently playing
    bool enginePlaying = false;
    bool turnPlaying = false;
    bool brakePlaying = false;

    // Track active effect channels (for MUSIC_ONLY filtering)
    std::unordered_set<int> activeEffectChannels;

    // Distance-based sound throttling
    Uint32 lastBrakeSoundTime = 0;
    const Uint32 BRAKE_SOUND_COOLDOWN_MS = 150;

    // Sound loading
    Mix_Chunk* loadSound(const std::string& filepath);
    void initAudio();

    static void onChannelFinished(int channel);

public:
    CarSoundEngine();
    ~CarSoundEngine();

    // Update sound based on what keys are currently pressed
    // Pass true/false for each action currently happening
    void update(bool isAccelerating, bool isTurning, bool isBraking);

    // Trigger specific sounds
    void playMusicTrack();
    void playAcceleration();
    void playTurnSound();
    void playBrakeSound();
    void playCollisionSound();
    void stopAcceleration();
    void stopTurn();
    void stopBrake();
    void stopAll();

    // Contextual sounds with distance modulation
    void playRaceFinish();      // When player finishes race (victory)
    void playGameOver();        // When player loses
    void playChampionshipWin();  // When player wins championship
    void playCheatActivated();  // When cheat is activated
    void playCountdownRace();   // When race countdown starts

private:
};

#endif  // CAR_SOUND_ENGINE_H
