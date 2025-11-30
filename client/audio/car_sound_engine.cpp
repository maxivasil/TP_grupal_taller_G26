#include "car_sound_engine.h"

#include <cmath>
#include <cstring>
#include <filesystem>
#include <iostream>

#include <SDL2/SDL.h>

#include "../../common/constants.h"

#define SOUND_DIR "sounds/"

CarSoundEngine* CarSoundEngine::instance = nullptr;

CarSoundEngine::CarSoundEngine():
        audioInitialized(false),
        engineAccelerationSound(nullptr),
        engineLoopSound(nullptr),
        turnSound(nullptr),
        brakeSound(nullptr),
        collisionSound(nullptr),
        raceFinishSound(nullptr),
        gameOverSound(nullptr),
        cheatActivatedSound(nullptr),
        countdownRaceSound(nullptr),
        musicTrack(nullptr) {
    instance = this;
    initAudio();
}

CarSoundEngine::~CarSoundEngine() {
    if (engineAccelerationSound)
        Mix_FreeChunk(engineAccelerationSound);
    if (engineLoopSound)
        Mix_FreeChunk(engineLoopSound);
    if (turnSound)
        Mix_FreeChunk(turnSound);
    if (brakeSound)
        Mix_FreeChunk(brakeSound);
    if (collisionSound)
        Mix_FreeChunk(collisionSound);
    if (raceFinishSound)
        Mix_FreeChunk(raceFinishSound);
    if (gameOverSound)
        Mix_FreeChunk(gameOverSound);
    if (cheatActivatedSound)
        Mix_FreeChunk(cheatActivatedSound);
    if (countdownRaceSound)
        Mix_FreeChunk(countdownRaceSound);
    if (musicTrack)
        Mix_FreeMusic(musicTrack);
    if (audioInitialized)
        Mix_CloseAudio();
}

void CarSoundEngine::initAudio() {
    if (audioInitialized)
        return;

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        std::cerr << "[CarSoundEngine] Failed to initialize audio: " << Mix_GetError() << std::endl;
        return;
    }

    audioInitialized = true;
    Mix_ChannelFinished(CarSoundEngine::onChannelFinished);
    std::string basePath = ABS_DIR ASSETS_DIR SOUND_DIR;

    engineAccelerationSound = loadSound(basePath + "car_engine_acceleration.wav");
    engineLoopSound = loadSound(basePath + "car_engine_acceleration_loop.wav");
    turnSound = loadSound(basePath + "tire_squeal_drift.mp3");
    brakeSound = loadSound(basePath + "car_brake_sound.mp3");
    collisionSound = loadSound(basePath + "collision_sound.mp3");
    raceFinishSound = loadSound(basePath + "victory_game.mp3");
    gameOverSound = loadSound(basePath + "game_over.mp3");
    cheatActivatedSound = loadSound(basePath + "cheat_activated.mp3");
    championshipWinSound = loadSound(basePath + "championship_win.mp3");
    countdownRaceSound = loadSound(basePath + "countdown_race.mp3");
    musicTrack = Mix_LoadMUS((basePath + "music_theme.mp3").c_str());

    playMusicTrack();
}

Mix_Chunk* CarSoundEngine::loadSound(const std::string& filepath) {
    Mix_Chunk* chunk = Mix_LoadWAV(filepath.c_str());
    if (!chunk) {
        std::cerr << "[CarSoundEngine] Failed to load sound: " << filepath
                  << " - Error: " << Mix_GetError() << std::endl;
        return nullptr;
    }

    return chunk;
}

void CarSoundEngine::update(bool isAccelerating, bool isTurning, bool isBraking) {
    if (!audioInitialized)
        return;

    if (isAccelerating && !enginePlaying) {
        enginePlaying = true;
        playAcceleration();
    } else if (enginePlaying && !isAccelerating) {
        enginePlaying = false;
        stopAcceleration();
    }

    if (isTurning && !turnPlaying) {
        playTurnSound();
        turnPlaying = true;
    } else if (turnPlaying && !isTurning) {
        stopTurn();
        turnPlaying = false;
    }

    if (isBraking && !brakePlaying) {
        playBrakeSound();
        brakePlaying = true;
    } else if (brakePlaying && !isBraking) {
        stopBrake();
        brakePlaying = false;
    }
}

void CarSoundEngine::playMusicTrack() {
    if (!audioInitialized || !musicTrack) {
        std::cerr << "[Sound] No music loaded: " << Mix_GetError() << std::endl;
        return;
    }

    if (Mix_PlayingMusic() == 0) {
        if (Mix_PlayMusic(musicTrack, -1) == -1) {
            std::cerr << "[Sound] Music play error: " << Mix_GetError() << std::endl;
        }
        Mix_VolumeMusic(int(MIX_MAX_VOLUME * 0.3f));
    }
}

void CarSoundEngine::playAcceleration() {
    if (!audioInitialized || !engineAccelerationSound || !engineLoopSound) {
        std::cerr << "[CarSoundEngine] Cannot play acceleration: audio not ready" << std::endl;
        return;
    }

    stopAcceleration();

    engineStartChannel = Mix_PlayChannel(-1, engineAccelerationSound, 0);
    if (engineStartChannel != -1) {
        activeEffectChannels.insert(engineStartChannel);
    }

    Mix_ChannelFinished(CarSoundEngine::onChannelFinished);
}

void CarSoundEngine::playTurnSound() {
    if (!audioInitialized || !turnSound) {
        std::cerr << "[CarSoundEngine] Cannot play turn sound: audio not ready" << std::endl;
        return;
    }

    stopTurn();

    int channel = Mix_PlayChannel(-1, turnSound, 0);
    if (channel == -1) {
        std::cerr << "[CarSoundEngine] ERROR! Failed to play turn sound: " << Mix_GetError()
                  << std::endl;
    } else {
        turnChannel = channel;
        activeEffectChannels.insert(channel);
    }
}

void CarSoundEngine::playBrakeSound() {
    if (!audioInitialized || !brakeSound) {
        std::cerr << "[CarSoundEngine] Cannot play brake sound: audio not ready" << std::endl;
        return;
    }

    stopBrake();

    int channel = Mix_PlayChannel(-1, brakeSound, 0);
    if (channel == -1) {
        std::cerr << "[CarSoundEngine] ERROR! Failed to play brake sound: " << Mix_GetError()
                  << std::endl;
    } else {
        brakeChannel = channel;
        activeEffectChannels.insert(channel);
    }
}

void CarSoundEngine::playCollisionSound() {
    if (!audioInitialized || !collisionSound) {
        std::cerr << "[CarSoundEngine] Cannot play collision sound: audio not ready" << std::endl;
        return;
    }

    if (collisionChannel != -1) {
        activeEffectChannels.erase(collisionChannel);
        Mix_HaltChannel(collisionChannel);
    }

    int channel = Mix_PlayChannel(-1, collisionSound, 0);
    if (channel == -1) {
        std::cerr << "[CarSoundEngine] ERROR! Failed to play collision sound: " << Mix_GetError()
                  << std::endl;
    } else {
        collisionChannel = channel;
        activeEffectChannels.insert(channel);
    }
}

void CarSoundEngine::playRaceFinish() {
    if (!audioInitialized || !raceFinishSound) {
        return;
    }

    if (contextSoundChannel != -1) {
        activeEffectChannels.erase(contextSoundChannel);
        Mix_HaltChannel(contextSoundChannel);
    }

    int channel = Mix_PlayChannel(-1, raceFinishSound, 0);
    if (channel == -1) {
        std::cerr << "[CarSoundEngine] Failed to play race finish sound: " << Mix_GetError()
                  << std::endl;
    } else {
        contextSoundChannel = channel;
        activeEffectChannels.insert(channel);
        Mix_Volume(channel, MIX_MAX_VOLUME);  // Full volume
    }
}

void CarSoundEngine::playGameOver() {
    if (!audioInitialized || !gameOverSound) {
        return;
    }

    if (contextSoundChannel != -1) {
        activeEffectChannels.erase(contextSoundChannel);
        Mix_HaltChannel(contextSoundChannel);
    }

    int channel = Mix_PlayChannel(-1, gameOverSound, 0);
    if (channel == -1) {
        std::cerr << "[CarSoundEngine] Failed to play game over sound: " << Mix_GetError()
                  << std::endl;
    } else {
        contextSoundChannel = channel;
        activeEffectChannels.insert(channel);
        Mix_Volume(channel, MIX_MAX_VOLUME);  // Full volume
    }
}

void CarSoundEngine::playChampionshipWin() {
    if (!audioInitialized || !championshipWinSound) {
        return;
    }

    stopAll();
    Mix_HaltMusic();

    int channel = Mix_PlayChannel(-1, championshipWinSound, 0);
    if (channel == -1) {
        std::cerr << "[CarSoundEngine] Failed to play championship win sound: " << Mix_GetError()
                  << std::endl;
    } else {
        contextSoundChannel = channel;
        activeEffectChannels.insert(channel);
        Mix_Volume(channel, MIX_MAX_VOLUME);
    }
}

void CarSoundEngine::playCheatActivated() {
    if (!audioInitialized || !cheatActivatedSound) {
        return;
    }

    if (contextSoundChannel != -1) {
        activeEffectChannels.erase(contextSoundChannel);
        Mix_HaltChannel(contextSoundChannel);
    }

    int channel = Mix_PlayChannel(-1, cheatActivatedSound, 0);
    if (channel == -1) {
        std::cerr << "[CarSoundEngine] Failed to play cheat activated sound: " << Mix_GetError()
                  << std::endl;
    } else {
        contextSoundChannel = channel;
        activeEffectChannels.insert(channel);
        Mix_Volume(channel, MIX_MAX_VOLUME * 0.8f);  // 80% volume
    }
}

void CarSoundEngine::playCountdownRace() {
    if (!audioInitialized || !countdownRaceSound) {
        return;
    }

    if (contextSoundChannel != -1) {
        activeEffectChannels.erase(contextSoundChannel);
        Mix_HaltChannel(contextSoundChannel);
    }

    int channel = Mix_PlayChannel(-1, countdownRaceSound, 0);
    if (channel == -1) {
        std::cerr << "[CarSoundEngine] Failed to play countdown race sound: " << Mix_GetError()
                  << std::endl;
    } else {
        contextSoundChannel = channel;
        activeEffectChannels.insert(channel);
        Mix_Volume(channel, MIX_MAX_VOLUME);  // Full volume
    }
}

void CarSoundEngine::onChannelFinished(int channel) {
    if (!instance)
        return;

    instance->activeEffectChannels.erase(channel);

    if (channel == instance->engineStartChannel && instance->enginePlaying) {
        instance->engineLoopChannel = Mix_PlayChannel(-1, instance->engineLoopSound, -1);
        if (instance->engineLoopChannel != -1) {
            instance->activeEffectChannels.insert(instance->engineLoopChannel);
        }
    }
}

void CarSoundEngine::stopAcceleration() {
    if (engineStartChannel != -1) {
        int channelToHalt = engineStartChannel;
        engineStartChannel = -1;
        activeEffectChannels.erase(channelToHalt);
        Mix_HaltChannel(channelToHalt);
    }

    if (engineLoopChannel != -1) {
        int channelToHalt = engineLoopChannel;
        engineLoopChannel = -1;
        activeEffectChannels.erase(channelToHalt);
        Mix_HaltChannel(channelToHalt);
    }
}

void CarSoundEngine::stopTurn() {
    if (turnChannel != -1) {
        activeEffectChannels.erase(turnChannel);
        Mix_HaltChannel(turnChannel);
        turnChannel = -1;
    }
}

void CarSoundEngine::stopBrake() {
    if (brakeChannel != -1) {
        activeEffectChannels.erase(brakeChannel);
        Mix_HaltChannel(brakeChannel);
        brakeChannel = -1;
    }
}

void CarSoundEngine::stopAll() {
    if (!audioInitialized)
        return;

    stopAcceleration();
    stopBrake();
    stopTurn();

    if (contextSoundChannel != -1) {
        Mix_HaltChannel(contextSoundChannel);
        activeEffectChannels.erase(contextSoundChannel);
        contextSoundChannel = -1;
    }

    for (int channel: activeEffectChannels) {
        Mix_HaltChannel(channel);
    }

    activeEffectChannels.clear();
}
