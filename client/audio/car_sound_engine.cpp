#include "car_sound_engine.h"
#include <cmath>
#include <iostream>
#include <cstring>
#include <filesystem>
#include <SDL2/SDL.h>

CarSoundEngine::CarSoundEngine()
    : audioInitialized(false),
      engineSound(nullptr),
      turnSound(nullptr),
      brakeSound(nullptr),
      collisionSound(nullptr),
      engineNoiseSound(nullptr),
      raceFinishSound(nullptr),
      gameOverSound(nullptr),
      cheatActivatedSound(nullptr),
      brakeLoudSound(nullptr),
      otherCarCollision(nullptr) {
    initAudio();
}

CarSoundEngine::~CarSoundEngine() {
    stopEngineNoise();
    if (engineSound) Mix_FreeChunk(engineSound);
    if (turnSound) Mix_FreeChunk(turnSound);
    if (brakeSound) Mix_FreeChunk(brakeSound);
    if (collisionSound) Mix_FreeChunk(collisionSound);
    if (engineNoiseSound) Mix_FreeChunk(engineNoiseSound);
    if (raceFinishSound) Mix_FreeChunk(raceFinishSound);
    if (gameOverSound) Mix_FreeChunk(gameOverSound);
    if (cheatActivatedSound) Mix_FreeChunk(cheatActivatedSound);
    if (brakeLoudSound) Mix_FreeChunk(brakeLoudSound);
    if (otherCarCollision) Mix_FreeChunk(otherCarCollision);
    if (audioInitialized) {
        Mix_CloseAudio();
    }
}

void CarSoundEngine::initAudio() {
    if (audioInitialized) return;
    
    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        std::cerr << "[CarSoundEngine] Failed to initialize audio: " << Mix_GetError() << std::endl;
        return;
    }
    
    audioInitialized = true;
    std::cout << "[CarSoundEngine] Audio initialized successfully" << std::endl;
    
    std::cout << "[CarSoundEngine] Attempting to load external sound files..." << std::endl;
    
    std::vector<std::string> basePaths = {
        "assets/sounds/",
        "../assets/sounds/",
        "../../assets/sounds/",
        "./assets/sounds/"
    };
    
    bool engineLoaded = false, turnLoaded = false, brakeLoaded = false, collisionLoaded = false, engineNoiseLoaded = false;
    bool finishLoaded = false, gameOverLoaded = false, cheatLoaded = false, brakeLoudLoaded = false, otherCarLoaded = false;
    
    for (const auto& basePath : basePaths) {
        if (!engineLoaded) {
            engineSound = loadSound(basePath + "car_engine_acceleration.wav");
            if (engineSound) {
                std::cout << "[CarSoundEngine] ✓ Engine sound loaded from " << basePath << std::endl;
                engineLoaded = true;
            }
        }
        
        if (!turnLoaded) {
            turnSound = loadSound(basePath + "tire_squeal_drift.mp3");
            if (turnSound) {
                std::cout << "[CarSoundEngine] ✓ Turn sound loaded from " << basePath << std::endl;
                turnLoaded = true;
            }
        }
        
        if (!brakeLoaded) {
            brakeSound = loadSound(basePath + "car_brake_sound.mp3");
            if (brakeSound) {
                std::cout << "[CarSoundEngine] ✓ Brake sound loaded from " << basePath << std::endl;
                brakeLoaded = true;
            }
        }
        
        if (!collisionLoaded) {
            collisionSound = loadSound(basePath + "collision_sound.mp3");
            if (collisionSound) {
                std::cout << "[CarSoundEngine] ✓ Collision sound loaded from " << basePath << std::endl;
                collisionLoaded = true;
            }
        }
        
        if (!engineNoiseLoaded) {
            engineNoiseSound = loadSound(basePath + "car_engine_noise.mp3");
            if (engineNoiseSound) {
                std::cout << "[CarSoundEngine] ✓ Engine noise sound loaded from " << basePath << std::endl;
                engineNoiseLoaded = true;
            }
        }
        
        if (!finishLoaded) {
            raceFinishSound = loadSound(basePath + "victory_game.mp3");
            if (raceFinishSound) {
                std::cout << "[CarSoundEngine] ✓ Victory sound loaded from " << basePath << std::endl;
                finishLoaded = true;
            }
        }
        
        if (!gameOverLoaded) {
            gameOverSound = loadSound(basePath + "game_over.mp3");
            if (gameOverSound) {
                std::cout << "[CarSoundEngine] ✓ Game over sound loaded from " << basePath << std::endl;
                gameOverLoaded = true;
            }
        }
        
        if (!cheatLoaded) {
            cheatActivatedSound = loadSound(basePath + "cheat_activated.mp3");
            if (cheatActivatedSound) {
                std::cout << "[CarSoundEngine] ✓ Cheat activated sound loaded from " << basePath << std::endl;
                cheatLoaded = true;
            }
        }
        
        if (!brakeLoudLoaded) {
            brakeLoudSound = loadSound(basePath + "brake_screech.mp3");
            if (brakeLoudSound) {
                std::cout << "[CarSoundEngine] ✓ Brake screech sound loaded from " << basePath << std::endl;
                brakeLoudLoaded = true;
            }
        }
        
        if (!otherCarLoaded) {
            otherCarCollision = loadSound(basePath + "other_car_collision.mp3");
            if (otherCarCollision) {
                std::cout << "[CarSoundEngine] ✓ Other car collision sound loaded from " << basePath << std::endl;
                otherCarLoaded = true;
            }
        }
        
        if (engineLoaded && turnLoaded && brakeLoaded && collisionLoaded && engineNoiseLoaded 
            && finishLoaded && gameOverLoaded && cheatLoaded && brakeLoudLoaded && otherCarLoaded) break;
    }
    
    if (!engineLoaded) {
        std::cout << "[CarSoundEngine] Engine sound file not found, generating procedurally..." << std::endl;
        engineSound = createEngineSound(1.0f);
    }
    
    if (!turnLoaded) {
        std::cout << "[CarSoundEngine] Turn sound file not found, generating procedurally..." << std::endl;
        turnSound = createTurnSound();
    }
    
    if (!brakeLoaded) {
        std::cout << "[CarSoundEngine] Brake sound file not found, generating procedurally..." << std::endl;
        brakeSound = createBrakeSound();
    }
    
    if (!collisionLoaded) {
        std::cout << "[CarSoundEngine] Collision sound file not found, generating procedurally..." << std::endl;
        collisionSound = createCollisionSound();
    }
    
    if (!engineNoiseLoaded) {
        std::cout << "[CarSoundEngine] Engine noise file not found, generating procedurally..." << std::endl;
        engineNoiseSound = createEngineSound(0.5f);  // Lower pitch for background noise
    }
    
    if (!finishLoaded) {
        std::cout << "[CarSoundEngine] Race finish sound not found, generating procedurally..." << std::endl;
        raceFinishSound = createVictorySound();
    }
    
    if (!brakeLoudLoaded) {
        std::cout << "[CarSoundEngine] Brake screech not found, will skip..." << std::endl;
    }
    
    if (!otherCarLoaded) {
        std::cout << "[CarSoundEngine] Other car collision not found, will skip..." << std::endl;
    }
    
    std::cout << "[CarSoundEngine] Sounds ready - Engine: " << (engineSound ? "OK" : "FAIL")
              << ", Turn: " << (turnSound ? "OK" : "FAIL")
              << ", Brake: " << (brakeSound ? "OK" : "FAIL")
              << ", Collision: " << (collisionSound ? "OK" : "FAIL")
              << ", EngineNoise: " << (engineNoiseSound ? "OK" : "FAIL")
              << ", Finish: " << (raceFinishSound ? "OK" : "SKIP")
              << ", BrakeLoud: " << (brakeLoudSound ? "OK" : "SKIP")
              << ", OtherCar: " << (otherCarCollision ? "OK" : "SKIP") << std::endl;
    
    playEngineNoise();
}

Mix_Chunk* CarSoundEngine::loadSound(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        return nullptr;
    }
    
    Mix_Chunk* chunk = Mix_LoadWAV(filepath.c_str());
    if (!chunk) {
        std::cerr << "[CarSoundEngine] Failed to load sound: " << filepath 
                  << " - Error: " << Mix_GetError() << std::endl;
        return nullptr;
    }
    
    return chunk;
}

Mix_Chunk* CarSoundEngine::createEngineSound(float pitch) {
    const int SAMPLE_RATE = 22050;
    const int DURATION_MS = 700;  
    const int SAMPLES = (SAMPLE_RATE * DURATION_MS) / 1000;
    
    Uint8* buffer = new Uint8[SAMPLES * 4]; 
    Sint16* samples = reinterpret_cast<Sint16*>(buffer);
    
    for (int i = 0; i < SAMPLES * 2; ++i) {
        int sampleIndex = i / 2;
        float progress = static_cast<float>(sampleIndex) / SAMPLES;
        
        float envelope = progress < 0.05f ? progress * 20.0f : 1.0f - (progress - 0.05f) * (1.0f / 0.95f);
        
        float baseFreq = 80.0f * pitch + progress * 120.0f * pitch;
        float harmonic2 = baseFreq * 2.0f;  
        float harmonic3 = baseFreq * 3.5f;  
        
        float angle1 = 2.0f * M_PI * baseFreq * sampleIndex / SAMPLE_RATE;
        float angle2 = 2.0f * M_PI * harmonic2 * sampleIndex / SAMPLE_RATE;
        float angle3 = 2.0f * M_PI * harmonic3 * sampleIndex / SAMPLE_RATE;
        
        float value = (sin(angle1) * 0.5f + sin(angle2) * 0.3f + sin(angle3) * 0.15f) * envelope;
        value = value * 0.95f; 
        
        samples[i] = static_cast<Sint16>(value * 32767.0f);
    }
    
    Mix_Chunk* chunk = Mix_QuickLoad_RAW(buffer, SAMPLES * 4);
    if (!chunk) {
        std::cerr << "[CarSoundEngine] Mix_QuickLoad_RAW failed: " << Mix_GetError() << std::endl;
        delete[] buffer;
    }
    return chunk;
}

Mix_Chunk* CarSoundEngine::createTurnSound() {
    const int SAMPLE_RATE = 22050;
    const int DURATION_MS = 400;  // Slightly longer for more dramatic effect
    const int SAMPLES = (SAMPLE_RATE * DURATION_MS) / 1000;
    
    Uint8* buffer = new Uint8[SAMPLES * 4];  // Stereo
    Sint16* samples = reinterpret_cast<Sint16*>(buffer);
    
    for (int i = 0; i < SAMPLES * 2; ++i) {
        int sampleIndex = i / 2;
        float progress = static_cast<float>(sampleIndex) / SAMPLES;
        
        float envelope = progress < 0.1f ? progress * 10.0f : 1.0f - (progress - 0.1f) * (1.0f / 0.9f);
        
        float freq1 = 1200.0f - progress * 300.0f;  // Main frequency sweep down
        float freq2 = 2000.0f - progress * 400.0f;  // Second frequency
        float freq3 = 800.0f + progress * 200.0f;   // Third frequency sweep up
        
        float wobble = 1.0f + 0.1f * sin(2.0f * M_PI * 5.0f * progress);
        
        float angle1 = 2.0f * M_PI * freq1 * sampleIndex / SAMPLE_RATE;
        float angle2 = 2.0f * M_PI * freq2 * sampleIndex / SAMPLE_RATE;
        float angle3 = 2.0f * M_PI * freq3 * sampleIndex / SAMPLE_RATE;
        
        float value = (sin(angle1) * 0.4f + sin(angle2) * 0.35f + sin(angle3) * 0.25f) * envelope * wobble;
        value = value * 0.95f;
        
        samples[i] = static_cast<Sint16>(value * 32767.0f);
    }
    
    Mix_Chunk* chunk = Mix_QuickLoad_RAW(buffer, SAMPLES * 4);
    if (!chunk) {
        std::cerr << "[CarSoundEngine] Mix_QuickLoad_RAW failed for turn sound: " << Mix_GetError() << std::endl;
        delete[] buffer;
    }
    return chunk;
}

Mix_Chunk* CarSoundEngine::createBrakeSound() {
    const int SAMPLE_RATE = 22050;
    const int DURATION_MS = 600;  // Longer for more dramatic braking
    const int SAMPLES = (SAMPLE_RATE * DURATION_MS) / 1000;
    
    Uint8* buffer = new Uint8[SAMPLES * 4];  // Stereo
    Sint16* samples = reinterpret_cast<Sint16*>(buffer);
    
    for (int i = 0; i < SAMPLES * 2; ++i) {
        int sampleIndex = i / 2;
        float progress = static_cast<float>(sampleIndex) / SAMPLES;
        
        float envelope = 1.0f - progress * progress;  // Quadratic decay
        
        float freq1 = 300.0f - progress * 150.0f;    // Main frequency descends
        float freq2 = 150.0f - progress * 75.0f;     // Sub-bass frequency
        float freq3 = 600.0f - progress * 300.0f;    // Higher component
        
        float pulse = 1.0f + 0.15f * sin(2.0f * M_PI * 8.0f * progress);
        
        float angle1 = 2.0f * M_PI * freq1 * sampleIndex / SAMPLE_RATE;
        float angle2 = 2.0f * M_PI * freq2 * sampleIndex / SAMPLE_RATE;
        float angle3 = 2.0f * M_PI * freq3 * sampleIndex / SAMPLE_RATE;
        
        float value = (sin(angle1) * 0.4f + sin(angle2) * 0.35f + sin(angle3) * 0.25f) * envelope * pulse;
        value = value * 0.95f;
        
        samples[i] = static_cast<Sint16>(value * 32767.0f);
    }
    
    Mix_Chunk* chunk = Mix_QuickLoad_RAW(buffer, SAMPLES * 4);
    if (!chunk) {
        std::cerr << "[CarSoundEngine] Mix_QuickLoad_RAW failed for brake sound: " << Mix_GetError() << std::endl;
        delete[] buffer;
    }
    return chunk;
}

Mix_Chunk* CarSoundEngine::createCollisionSound() {
    const int SAMPLE_RATE = 22050;
    const int DURATION_MS = 300; 
    const int SAMPLES = (SAMPLE_RATE * DURATION_MS) / 1000;
    
    Uint8* buffer = new Uint8[SAMPLES * 4];  
    Sint16* samples = reinterpret_cast<Sint16*>(buffer);
    
    for (int i = 0; i < SAMPLES * 2; ++i) {
        int sampleIndex = i / 2;
        float progress = static_cast<float>(sampleIndex) / SAMPLES;
        
        float envelope = progress < 0.05f ? progress * 20.0f : 1.0f - (progress - 0.05f) * (1.0f / 0.95f);
        
        float freq1 = 200.0f - progress * 100.0f;    // Low frequency descends
        float freq2 = 400.0f - progress * 200.0f;    // Mid frequency
        float freq3 = 1000.0f - progress * 500.0f;   // High frequency
        
        float angle1 = 2.0f * M_PI * freq1 * sampleIndex / SAMPLE_RATE;
        float angle2 = 2.0f * M_PI * freq2 * sampleIndex / SAMPLE_RATE;
        float angle3 = 2.0f * M_PI * freq3 * sampleIndex / SAMPLE_RATE;
        
        float value = (sin(angle1) * 0.35f + sin(angle2) * 0.4f + sin(angle3) * 0.25f) * envelope;
        value = value * 0.95f;
        
        samples[i] = static_cast<Sint16>(value * 32767.0f);
    }
    
    Mix_Chunk* chunk = Mix_QuickLoad_RAW(buffer, SAMPLES * 4);
    if (!chunk) {
        std::cerr << "[CarSoundEngine] Mix_QuickLoad_RAW failed for collision sound: " << Mix_GetError() << std::endl;
        delete[] buffer;
    }
    return chunk;
}

void CarSoundEngine::update(bool isAccelerating, bool isTurning, bool isBraking) {
    if (!audioInitialized) return;
    
    if (isAccelerating) {
        if (!enginePlaying) {
            std::cout << "[CarSoundEngine] ▶ Engine sound ON (key held down)" << std::endl;
            playAcceleration();
            enginePlaying = true;
        }
    } else {
        if (enginePlaying) {
            std::cout << "[CarSoundEngine] ⏹ Engine sound OFF (key released)" << std::endl;
            stopAcceleration();
            enginePlaying = false;
        }
    }
    
    if (isTurning) {
        if (!turnPlaying) {
            std::cout << "[CarSoundEngine] ▶ Turn sound ON (key held down)" << std::endl;
            playTurnSound();
            turnPlaying = true;
        }
    } else {
        if (turnPlaying) {
            std::cout << "[CarSoundEngine] ⏹ Turn sound OFF (key released)" << std::endl;
            stopTurn();
            turnPlaying = false;
        }
    }
    
    if (isBraking) {
        if (!brakePlaying) {
            std::cout << "[CarSoundEngine] ▶ Brake sound ON (key held down)" << std::endl;
            playBrakeSound();
            brakePlaying = true;
        }
    } else {
        if (brakePlaying) {
            std::cout << "[CarSoundEngine] ⏹ Brake sound OFF (key released)" << std::endl;
            stopBrake();
            brakePlaying = false;
        }
    }
}

void CarSoundEngine::playAcceleration() {
    if (!audioInitialized || !engineSound) {
        std::cerr << "[CarSoundEngine] Cannot play acceleration: audio not ready" << std::endl;
        return;
    }
    
    stopAcceleration();
    
    int channel = Mix_PlayChannel(-1, engineSound, 0);
    if (channel == -1) {
        std::cerr << "[CarSoundEngine] ERROR! Failed to play acceleration: " << Mix_GetError() << std::endl;
    } else {
        engineChannel = channel;
        std::cout << "[CarSoundEngine] ▶ Engine sound playing on channel " << channel << std::endl;
    }
}

void CarSoundEngine::playTurnSound() {
    if (!audioInitialized || !turnSound) {
        std::cerr << "[CarSoundEngine] Cannot play turn sound: audio not ready" << std::endl;
        return;
    }
    
    stopTurn();
    
    int channel = Mix_PlayChannel(-1, turnSound, 0);
    if (channel == -1) {
        std::cerr << "[CarSoundEngine] ERROR! Failed to play turn sound: " << Mix_GetError() << std::endl;
    } else {
        turnChannel = channel;
        std::cout << "[CarSoundEngine] ▶ Turn sound playing on channel " << channel << std::endl;
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
        std::cerr << "[CarSoundEngine] ERROR! Failed to play brake sound: " << Mix_GetError() << std::endl;
    } else {
        brakeChannel = channel;
        std::cout << "[CarSoundEngine] ▶ Brake sound playing on channel " << channel << std::endl;
    }
}

void CarSoundEngine::playCollisionSound() {
    if (!audioInitialized || !collisionSound) {
        std::cerr << "[CarSoundEngine] Cannot play collision sound: audio not ready" << std::endl;
        return;
    }
    
    if (collisionChannel != -1) {
        Mix_HaltChannel(collisionChannel);
    }
    
    int channel = Mix_PlayChannel(-1, collisionSound, 0);
    if (channel == -1) {
        std::cerr << "[CarSoundEngine] ERROR! Failed to play collision sound: " << Mix_GetError() << std::endl;
    } else {
        collisionChannel = channel;
        std::cout << "[CarSoundEngine] COLLISION sound playing on channel " << channel << std::endl;
    }
}

void CarSoundEngine::stopAcceleration() {
    if (engineChannel != -1) {
        Mix_HaltChannel(engineChannel);
        engineChannel = -1;
    }
}

void CarSoundEngine::stopTurn() {
    if (turnChannel != -1) {
        Mix_HaltChannel(turnChannel);
        turnChannel = -1;
    }
}

void CarSoundEngine::stopBrake() {
    if (brakeChannel != -1) {
        Mix_HaltChannel(brakeChannel);
        brakeChannel = -1;
    }
}

void CarSoundEngine::stopAll() {
    if (!audioInitialized) return;
    Mix_HaltChannel(-1); 
}

void CarSoundEngine::playEngineNoise() {
    if (!audioInitialized || !engineNoiseSound) {
        std::cerr << "[CarSoundEngine] Cannot play engine noise: audio not ready" << std::endl;
        return;
    }
    
    if (engineNoisePlaying) {
        return;
    }
    
    int channel = Mix_PlayChannel(-1, engineNoiseSound, -1);
    if (channel == -1) {
        std::cerr << "[CarSoundEngine] ERROR! Failed to play engine noise: " << Mix_GetError() << std::endl;
    } else {
        engineNoiseChannel = channel;
        engineNoisePlaying = true;
        Mix_Volume(channel, MIX_MAX_VOLUME * 0.6);  // 60% volume
        std::cout << "[CarSoundEngine] Engine noise started (looped) on channel " << channel << std::endl;
    }
}

void CarSoundEngine::stopEngineNoise() {
    if (engineNoiseChannel != -1) {
        Mix_HaltChannel(engineNoiseChannel);
        engineNoiseChannel = -1;
        engineNoisePlaying = false;
        std::cout << "[CarSoundEngine] Engine noise stopped" << std::endl;
    }
}

void CarSoundEngine::playRaceFinish() {
    if (!audioInitialized || !raceFinishSound) {
        std::cout << "[CarSoundEngine] Race finish sound not available" << std::endl;
        return;
    }
    
    if (contextSoundChannel != -1) {
        Mix_HaltChannel(contextSoundChannel);
    }
    
    int channel = Mix_PlayChannel(-1, raceFinishSound, 0);
    if (channel == -1) {
        std::cerr << "[CarSoundEngine] Failed to play race finish sound: " << Mix_GetError() << std::endl;
    } else {
        contextSoundChannel = channel;
        Mix_Volume(channel, MIX_MAX_VOLUME);  // Full volume
        std::cout << "[CarSoundEngine] 🏁 RACE FINISH sound playing on channel " << channel << std::endl;
    }
}

void CarSoundEngine::playGameOver() {
    if (!audioInitialized || !gameOverSound) {
        std::cout << "[CarSoundEngine] Game over sound not available" << std::endl;
        return;
    }
    
    if (contextSoundChannel != -1) {
        Mix_HaltChannel(contextSoundChannel);
    }
    
    int channel = Mix_PlayChannel(-1, gameOverSound, 0);
    if (channel == -1) {
        std::cerr << "[CarSoundEngine] Failed to play game over sound: " << Mix_GetError() << std::endl;
    } else {
        contextSoundChannel = channel;
        Mix_Volume(channel, MIX_MAX_VOLUME);  // Full volume
        std::cout << "[CarSoundEngine] ☠️ GAME OVER sound playing on channel " << channel << std::endl;
    }
}

void CarSoundEngine::playCheatActivated() {
    if (!audioInitialized || !cheatActivatedSound) {
        std::cout << "[CarSoundEngine] Cheat activated sound not available" << std::endl;
        return;
    }
    
    if (contextSoundChannel != -1) {
        Mix_HaltChannel(contextSoundChannel);
    }
    
    int channel = Mix_PlayChannel(-1, cheatActivatedSound, 0);
    if (channel == -1) {
        std::cerr << "[CarSoundEngine] Failed to play cheat activated sound: " << Mix_GetError() << std::endl;
    } else {
        contextSoundChannel = channel;
        Mix_Volume(channel, MIX_MAX_VOLUME * 0.8f);  // 80% volume
        std::cout << "[CarSoundEngine] 🎮 CHEAT ACTIVATED sound playing on channel " << channel << std::endl;
    }
}

void CarSoundEngine::playOtherCarCollision(float distance, float intensity) {
    if (!audioInitialized || !otherCarCollision) {
        return;  
    }
    
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastCollisionSoundTime < COLLISION_SOUND_COOLDOWN_MS) {
        return;
    }
    lastCollisionSoundTime = currentTime;
    
    float volumeFactor = calculateDistanceVolume(distance, 80.0f);
    if (volumeFactor < 0.1f) {
        return;  
    }
    
    volumeFactor *= (intensity / 20.0f);
    volumeFactor = std::min(1.0f, volumeFactor);
    
    int channel = Mix_PlayChannel(-1, otherCarCollision, 0);
    if (channel == -1) {
        std::cerr << "[CarSoundEngine] Failed to play other car collision: " << Mix_GetError() << std::endl;
    } else {
        otherCarChannel = channel;
        int volume = static_cast<int>(MIX_MAX_VOLUME * volumeFactor);
        Mix_Volume(channel, volume);
        std::cout << "[CarSoundEngine] Other car collision at distance " << distance 
                  << " with volume " << volume << " on channel " << channel << std::endl;
    }
}

void CarSoundEngine::playDistantBrake(float distance) {
    if (!audioInitialized || !brakeLoudSound) {
        return;  
    }
    
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastBrakeSoundTime < BRAKE_SOUND_COOLDOWN_MS) {
        return;
    }
    lastBrakeSoundTime = currentTime;
    
    float volumeFactor = calculateDistanceVolume(distance, 60.0f);
    if (volumeFactor < 0.05f) {
        return;  
    }
    
    int channel = Mix_PlayChannel(-1, brakeLoudSound, 0);
    if (channel == -1) {
        return; 
    } else {
        otherCarChannel = channel;
        int volume = static_cast<int>(MIX_MAX_VOLUME * volumeFactor);
        Mix_Volume(channel, volume);
        std::cout << "[CarSoundEngine] Distant brake at distance " << distance << std::endl;
    }
}

float CarSoundEngine::calculateDistanceVolume(float distance, float maxDistance) {
    if (distance <= 0.0f) {
        return 1.0f;  
    }
    if (distance >= maxDistance) {
        return 0.0f;  
    }
    
    float normalized = 1.0f - (distance / maxDistance);
    return normalized * normalized; 
}

Mix_Chunk* CarSoundEngine::createVictorySound() {
    // Create a celebratory "victory" chord sound
    const int SAMPLE_RATE = 22050;
    const int DURATION_MS = 1500;  // 1.5 seconds for victory fanfare
    const int SAMPLES = (SAMPLE_RATE * DURATION_MS) / 1000;
    
    Uint8* buffer = new Uint8[SAMPLES * 4];  
    Sint16* samples = reinterpret_cast<Sint16*>(buffer);
    
    // Create a rising chord progression (C-E-G major chord rising in pitch)
    const float notes[] = {261.63f, 329.63f, 392.0f};  // C, E, G (major chord)
    
    for (int i = 0; i < SAMPLES * 2; ++i) {
        int sampleIndex = i / 2;
        float progress = static_cast<float>(sampleIndex) / SAMPLES;
        
        // Envelope: attack (100ms) -> sustain -> release (300ms)
        float envelope;
        if (progress < 0.07f) {
            envelope = progress / 0.07f;  // Attack
        } else if (progress > 0.85f) {
            envelope = (1.0f - progress) / 0.15f;  // Release
        } else {
            envelope = 1.0f;  // Sustain
        }
        
        // Rising frequency modulation for excitement
        float freqMod = 1.0f + progress * 0.5f;  // Gradually rise in pitch
        
        float value = 0.0f;
        
        // Mix the three notes of the major chord with rising pitch
        for (int n = 0; n < 3; ++n) {
            float freq = notes[n] * freqMod;
            float angle = 2.0f * M_PI * freq * sampleIndex / SAMPLE_RATE;
            value += sin(angle) * (0.3f / 3.0f);  // Equal contribution from each note
        }
        
        // Add some brightness with harmonics
        float brightAngle = 2.0f * M_PI * (587.33f * freqMod) * sampleIndex / SAMPLE_RATE;
        value += sin(brightAngle) * 0.1f * envelope;
        
        value *= envelope;
        value = std::min(1.0f, std::max(-1.0f, value));  // Clamp
        
        samples[i] = static_cast<Sint16>(value * 32767.0f * 0.9f);
    }
    
    Mix_Chunk* chunk = Mix_QuickLoad_RAW(buffer, SAMPLES * 4);
    if (!chunk) {
        std::cerr << "[CarSoundEngine] Mix_QuickLoad_RAW failed for victory sound: " << Mix_GetError() << std::endl;
        delete[] buffer;
    }
    return chunk;
}
