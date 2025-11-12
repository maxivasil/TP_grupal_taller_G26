#include "car_sound_engine.h"
#include <cmath>
#include <iostream>
#include <cstring>
#include <filesystem>

CarSoundEngine::CarSoundEngine()
    : audioInitialized(false),
      engineSound(nullptr),
      turnSound(nullptr),
      brakeSound(nullptr) {
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
    
    // Try to load external sound files first
    std::cout << "[CarSoundEngine] Attempting to load external sound files..." << std::endl;
    
    // Try different paths for sound files
    std::vector<std::string> basePaths = {
        "assets/sounds/",
        "../assets/sounds/",
        "../../assets/sounds/",
        "./assets/sounds/"
    };
    
    bool engineLoaded = false, turnLoaded = false, brakeLoaded = false;
    
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
        
        if (engineLoaded && turnLoaded && brakeLoaded) break;
    }
    
    // If any sound files weren't loaded, generate them procedurally as fallback
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
    
    std::cout << "[CarSoundEngine] Sounds ready - Engine: " << (engineSound ? "OK" : "FAIL")
              << ", Turn: " << (turnSound ? "OK" : "FAIL")
              << ", Brake: " << (brakeSound ? "OK" : "FAIL") << std::endl;
}

Mix_Chunk* CarSoundEngine::loadSound(const std::string& filepath) {
    // Check if file exists first
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
    // Create realistic engine "vroom" sound with rising pitch (acceleration)
    const int SAMPLE_RATE = 22050;
    const int DURATION_MS = 700;  // Longer for more realistic vroom
    const int SAMPLES = (SAMPLE_RATE * DURATION_MS) / 1000;
    
    // Allocate buffer for 16-bit stereo audio
    Uint8* buffer = new Uint8[SAMPLES * 4];  // 2 channels * 2 bytes (16-bit)
    Sint16* samples = reinterpret_cast<Sint16*>(buffer);
    
    // Generate engine sound with multiple harmonics and frequency sweep
    for (int i = 0; i < SAMPLES * 2; ++i) {
        int sampleIndex = i / 2;
        float progress = static_cast<float>(sampleIndex) / SAMPLES;
        
        // Envelope: quick attack, gradual decay (vroom!)
        float envelope = progress < 0.05f ? progress * 20.0f : 1.0f - (progress - 0.05f) * (1.0f / 0.95f);
        
        // Frequency sweep: 80Hz -> 200Hz (rising engine pitch)
        float baseFreq = 80.0f * pitch + progress * 120.0f * pitch;
        float harmonic2 = baseFreq * 2.0f;  // 2nd harmonic
        float harmonic3 = baseFreq * 3.5f;  // 3.5th harmonic (less pure, more natural)
        
        // Generate three sine waves for richer, more organic sound
        float angle1 = 2.0f * M_PI * baseFreq * sampleIndex / SAMPLE_RATE;
        float angle2 = 2.0f * M_PI * harmonic2 * sampleIndex / SAMPLE_RATE;
        float angle3 = 2.0f * M_PI * harmonic3 * sampleIndex / SAMPLE_RATE;
        
        // Mix harmonics with different amplitudes (natural timbre)
        float value = (sin(angle1) * 0.5f + sin(angle2) * 0.3f + sin(angle3) * 0.15f) * envelope;
        value = value * 0.95f;  // Normalize
        
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
    // Create tire squeal/drift sound: sharp, high-frequency noise-like sound
    const int SAMPLE_RATE = 22050;
    const int DURATION_MS = 400;  // Slightly longer for more dramatic effect
    const int SAMPLES = (SAMPLE_RATE * DURATION_MS) / 1000;
    
    Uint8* buffer = new Uint8[SAMPLES * 4];  // Stereo
    Sint16* samples = reinterpret_cast<Sint16*>(buffer);
    
    // Tire squeal with multiple high frequencies
    for (int i = 0; i < SAMPLES * 2; ++i) {
        int sampleIndex = i / 2;
        float progress = static_cast<float>(sampleIndex) / SAMPLES;
        
        // Envelope: quick attack, medium decay with slight wobble
        float envelope = progress < 0.1f ? progress * 10.0f : 1.0f - (progress - 0.1f) * (1.0f / 0.9f);
        
        // Multiple squealing frequencies for harsh tire sound
        float freq1 = 1200.0f - progress * 300.0f;  // Main frequency sweep down
        float freq2 = 2000.0f - progress * 400.0f;  // Second frequency
        float freq3 = 800.0f + progress * 200.0f;   // Third frequency sweep up
        
        // Add slight modulation for "wobble"
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
    // Create braking sound: low rumbling descending tone (engine compression/braking)
    const int SAMPLE_RATE = 22050;
    const int DURATION_MS = 600;  // Longer for more dramatic braking
    const int SAMPLES = (SAMPLE_RATE * DURATION_MS) / 1000;
    
    Uint8* buffer = new Uint8[SAMPLES * 4];  // Stereo
    Sint16* samples = reinterpret_cast<Sint16*>(buffer);
    
    // Braking: multiple low frequencies with modulation
    for (int i = 0; i < SAMPLES * 2; ++i) {
        int sampleIndex = i / 2;
        float progress = static_cast<float>(sampleIndex) / SAMPLES;
        
        // Envelope: quick attack, long decay with slight pulse
        float envelope = 1.0f - progress * progress;  // Quadratic decay
        
        // Multiple descending frequencies for realistic braking sound
        float freq1 = 300.0f - progress * 150.0f;    // Main frequency descends
        float freq2 = 150.0f - progress * 75.0f;     // Sub-bass frequency
        float freq3 = 600.0f - progress * 300.0f;    // Higher component
        
        // Add slight "pulsing" modulation for engine compression effect
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

void CarSoundEngine::update(bool isAccelerating, bool isTurning, bool isBraking) {
    if (!audioInitialized) return;
    
    // ENGINE/ACCELERATION SOUND
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
    
    // TURN SOUND
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
    
    // BRAKE SOUND
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
    
    // Stop previous engine sound if any
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
    
    // Stop previous turn sound if any
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
    
    // Stop previous brake sound if any
    stopBrake();
    
    int channel = Mix_PlayChannel(-1, brakeSound, 0);
    if (channel == -1) {
        std::cerr << "[CarSoundEngine] ERROR! Failed to play brake sound: " << Mix_GetError() << std::endl;
    } else {
        brakeChannel = channel;
        std::cout << "[CarSoundEngine] ▶ Brake sound playing on channel " << channel << std::endl;
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
    Mix_HaltChannel(-1);  // Halt all channels
}
