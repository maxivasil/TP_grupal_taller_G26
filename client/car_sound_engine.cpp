#include "car_sound_engine.h"
#include <cmath>
#include <iostream>
#include <cstring>

CarSoundEngine::CarSoundEngine()
    : audioInitialized(false),
      engineSound(nullptr),
      turnSound(nullptr),
      brakeSound(nullptr),
      currentChannel(-1),
      currentState(CarSoundState::IDLE),
      currentSpeed(0.0f),
      previousSpeed(0.0f),
      previousTurning(false),
      previousBraking(false) {
    initAudio();
}

CarSoundEngine::~CarSoundEngine() {
    if (engineSound) Mix_FreeChunk(engineSound);
    if (turnSound) Mix_FreeChunk(turnSound);
    if (brakeSound) Mix_FreeChunk(brakeSound);
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
    
    // Pre-generate sounds
    engineSound = createEngineSound(1.0f);
    turnSound = createTurnSound();
    brakeSound = createBrakeSound();
    
    std::cout << "[CarSoundEngine] Sounds generated - Engine: " << (engineSound ? "OK" : "FAIL")
              << ", Turn: " << (turnSound ? "OK" : "FAIL")
              << ", Brake: " << (brakeSound ? "OK" : "FAIL") << std::endl;
}

Mix_Chunk* CarSoundEngine::createEngineSound(float pitch) {
    // Create engine sound: low-frequency rumble that varies with speed
    const int SAMPLE_RATE = 22050;
    const int DURATION_MS = 500;  // Longer duration
    const int SAMPLES = (SAMPLE_RATE * DURATION_MS) / 1000;
    
    // Allocate buffer for 16-bit stereo audio
    Uint8* buffer = new Uint8[SAMPLES * 4];  // 2 channels * 2 bytes (16-bit)
    Sint16* samples = reinterpret_cast<Sint16*>(buffer);
    
    // Generate engine sound: combination of frequencies
    float baseFreq = 150.0f * pitch;  // Base engine frequency
    float harmonic = 300.0f * pitch;  // Second harmonic
    
    for (int i = 0; i < SAMPLES * 2; ++i) {
        int sampleIndex = i / 2;
        float progress = static_cast<float>(sampleIndex) / SAMPLES;
        
        // Envelope: quick attack, slow decay
        float envelope = progress < 0.1f ? progress * 10.0f : 1.0f - (progress - 0.1f) * (1.0f / 0.9f);
        
        // Combine two sine waves for richer sound
        float angle1 = 2.0f * M_PI * baseFreq * sampleIndex / SAMPLE_RATE;
        float angle2 = 2.0f * M_PI * harmonic * sampleIndex / SAMPLE_RATE;
        
        float value = (sin(angle1) * 0.6f + sin(angle2) * 0.4f) / 2.0f * envelope;
        samples[i] = static_cast<Sint16>(value * 32767.0f * 0.8f);  // 80% amplitude
    }
    
    SDL_AudioSpec spec;
    SDL_zero(spec);
    spec.freq = SAMPLE_RATE;
    spec.format = AUDIO_S16SYS;
    spec.channels = 2;
    spec.samples = 1024;
    
    Mix_Chunk* chunk = Mix_QuickLoad_RAW(buffer, SAMPLES * 4);
    if (!chunk) {
        std::cerr << "[CarSoundEngine] Mix_QuickLoad_RAW failed: " << Mix_GetError() << std::endl;
        delete[] buffer;
    }
    return chunk;
}

Mix_Chunk* CarSoundEngine::createTurnSound() {
    // Create turn/squeal sound: high-frequency chirp
    const int SAMPLE_RATE = 22050;
    const int DURATION_MS = 300;  // Longer duration
    const int SAMPLES = (SAMPLE_RATE * DURATION_MS) / 1000;
    
    Uint8* buffer = new Uint8[SAMPLES * 4];  // Stereo
    Sint16* samples = reinterpret_cast<Sint16*>(buffer);
    
    // High-pitched squeal that rises and falls
    for (int i = 0; i < SAMPLES * 2; ++i) {
        int sampleIndex = i / 2;
        float progress = static_cast<float>(sampleIndex) / SAMPLES;
        
        // Envelope: quick attack, medium decay
        float envelope = progress < 0.05f ? progress * 20.0f : 1.0f - (progress - 0.05f) * (1.0f / 0.95f);
        
        // Frequency sweep from 1000 Hz to 800 Hz
        float freq = 1000.0f - progress * 200.0f;
        float angle = 2.0f * M_PI * freq * sampleIndex / SAMPLE_RATE;
        
        float value = sin(angle) * envelope;
        samples[i] = static_cast<Sint16>(value * 32767.0f * 0.6f);  // 60% amplitude
    }
    
    Mix_Chunk* chunk = Mix_QuickLoad_RAW(buffer, SAMPLES * 4);
    if (!chunk) {
        std::cerr << "[CarSoundEngine] Mix_QuickLoad_RAW failed for turn sound: " << Mix_GetError() << std::endl;
        delete[] buffer;
    }
    return chunk;
}

Mix_Chunk* CarSoundEngine::createBrakeSound() {
    // Create braking sound: descending tone
    const int SAMPLE_RATE = 22050;
    const int DURATION_MS = 400;  // Longer duration
    const int SAMPLES = (SAMPLE_RATE * DURATION_MS) / 1000;
    
    Uint8* buffer = new Uint8[SAMPLES * 4];  // Stereo
    Sint16* samples = reinterpret_cast<Sint16*>(buffer);
    
    // Braking: lower frequency that descends
    for (int i = 0; i < SAMPLES * 2; ++i) {
        int sampleIndex = i / 2;
        float progress = static_cast<float>(sampleIndex) / SAMPLES;
        
        // Envelope: quick attack, long decay
        float envelope = 1.0f - progress * progress;  // Quadratic decay
        
        // Frequency sweep from 400 Hz to 200 Hz
        float freq = 400.0f - progress * 200.0f;
        float angle = 2.0f * M_PI * freq * sampleIndex / SAMPLE_RATE;
        
        float value = sin(angle) * envelope;
        samples[i] = static_cast<Sint16>(value * 32767.0f * 0.7f);  // 70% amplitude
    }
    
    Mix_Chunk* chunk = Mix_QuickLoad_RAW(buffer, SAMPLES * 4);
    if (!chunk) {
        std::cerr << "[CarSoundEngine] Mix_QuickLoad_RAW failed for brake sound: " << Mix_GetError() << std::endl;
        delete[] buffer;
    }
    return chunk;
}

void CarSoundEngine::update(float speed, bool isTurning, bool isBraking) {
    if (!audioInitialized) return;
    
    currentSpeed = speed;
    
    // Debug: print current state
    std::cout << "[CarSoundEngine] Speed: " << speed 
              << " | Turning: " << isTurning << " (was " << previousTurning << ")"
              << " | Braking: " << isBraking << " (was " << previousBraking << ")" << std::endl;
    
    // DETECTION 1: Speed just became non-zero (acceleration)
    if (currentSpeed > 0.0f && previousSpeed == 0.0f) {
        std::cout << "[CarSoundEngine] ACCELERATION DETECTED! Speed went from 0 to " << speed << std::endl;
        playAcceleration();
    }
    
    // DETECTION 2: Turning state CHANGED (not already turning, now turning)
    if (isTurning && !previousTurning && currentSpeed > 0.0f) {
        std::cout << "[CarSoundEngine] TURN DETECTED! Speed: " << speed << std::endl;
        playTurnSound();
    }
    
    // DETECTION 3: Braking state CHANGED (not already braking, now braking)
    if (isBraking && !previousBraking && currentSpeed > 0.0f) {
        std::cout << "[CarSoundEngine] BRAKE DETECTED! Speed: " << speed << std::endl;
        playBrakeSound();
    }
    
    // Update previous state for next frame
    previousSpeed = currentSpeed;
    previousTurning = isTurning;
    previousBraking = isBraking;
}

void CarSoundEngine::playAcceleration() {
    if (!audioInitialized || !engineSound) {
        std::cerr << "[CarSoundEngine] Cannot play acceleration: audio not ready" << std::endl;
        return;
    }
    
    std::cout << "[CarSoundEngine] >>> PLAYING ENGINE SOUND <<<" << std::endl;
    int channel = Mix_PlayChannel(-1, engineSound, 0);
    if (channel == -1) {
        std::cerr << "[CarSoundEngine] ERROR! Failed to play acceleration: " << Mix_GetError() << std::endl;
    } else {
        std::cout << "[CarSoundEngine] ✓ Acceleration sound playing on channel " << channel << std::endl;
    }
}

void CarSoundEngine::playTurnSound() {
    if (!audioInitialized || !turnSound) {
        std::cerr << "[CarSoundEngine] Cannot play turn sound: audio not ready" << std::endl;
        return;
    }
    
    std::cout << "[CarSoundEngine] >>> PLAYING TURN SOUND <<<" << std::endl;
    int channel = Mix_PlayChannel(-1, turnSound, 0);
    if (channel == -1) {
        std::cerr << "[CarSoundEngine] ERROR! Failed to play turn sound: " << Mix_GetError() << std::endl;
    } else {
        std::cout << "[CarSoundEngine] ✓ Turn sound playing on channel " << channel << std::endl;
    }
}

void CarSoundEngine::playBrakeSound() {
    if (!audioInitialized || !brakeSound) {
        std::cerr << "[CarSoundEngine] Cannot play brake sound: audio not ready" << std::endl;
        return;
    }
    
    std::cout << "[CarSoundEngine] >>> PLAYING BRAKE SOUND <<<" << std::endl;
    int channel = Mix_PlayChannel(-1, brakeSound, 0);
    if (channel == -1) {
        std::cerr << "[CarSoundEngine] ERROR! Failed to play brake sound: " << Mix_GetError() << std::endl;
    } else {
        std::cout << "[CarSoundEngine] ✓ Brake sound playing on channel " << channel << std::endl;
    }
}

void CarSoundEngine::stop() {
    if (!audioInitialized) return;
    Mix_HaltChannel(-1);
}
