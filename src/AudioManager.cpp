#include "AudioManager.h"
#include <iostream>
#include <cmath>
#include <cstring>

AudioManager::AudioManager() {}

AudioManager::~AudioManager() {
    shutdown();
}

bool AudioManager::initialize() {
    std::cout << "Initializing audio manager..." << std::endl;
    
    // Configure audio spec
    SDL_AudioSpec desired;
    SDL_zero(desired);
    desired.freq = 44100;
    desired.format = AUDIO_F32SYS;
    desired.channels = 2;
    desired.samples = 2048;
    desired.callback = audioCallback;
    desired.userdata = this;
    
    audioDevice = SDL_OpenAudioDevice(nullptr, 0, &desired, &audioSpec, 0);
    
    if (audioDevice == 0) {
        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Start audio playback
    SDL_PauseAudioDevice(audioDevice, 0);
    
    isInitialized = true;
    std::cout << "Audio initialized: " << audioSpec.freq << " Hz, " 
              << (int)audioSpec.channels << " channels" << std::endl;
    
    return true;
}

void AudioManager::shutdown() {
    if (audioDevice != 0) {
        SDL_CloseAudioDevice(audioDevice);
        audioDevice = 0;
    }
    isInitialized = false;
}

void AudioManager::audioCallback(void* userdata, Uint8* stream, int len) {
    AudioManager* audio = static_cast<AudioManager*>(userdata);
    audio->mixAudio(stream, len);
}

void AudioManager::mixAudio(Uint8* stream, int len) {
    float* buffer = reinterpret_cast<float*>(stream);
    int sampleCount = len / sizeof(float) / 2;  // Stereo
    
    // Clear buffer
    std::memset(stream, 0, len);
    
    if (muted) return;
    
    float volume = masterVolume;
    
    // Generate engine sound
    float engineVol = engineVolume * volume * currentEngineThrottle * 0.3f;
    if (engineVol > 0.01f) {
        for (int i = 0; i < sampleCount; i++) {
            // Base engine frequency varies with throttle
            float baseFreq = 80.0f + currentEngineThrottle * 120.0f;
            
            // Generate engine sound with multiple harmonics
            float sample = 0.0f;
            sample += std::sin(enginePhase) * 0.5f;
            sample += std::sin(enginePhase * 2.0f) * 0.3f;
            sample += std::sin(enginePhase * 3.0f) * 0.15f;
            sample += std::sin(enginePhase * 0.5f) * 0.2f;
            
            // Add some noise for realism
            sample += ((float)rand() / RAND_MAX - 0.5f) * 0.1f;
            
            sample *= engineVol;
            
            // Stereo output
            buffer[i * 2] += sample;
            buffer[i * 2 + 1] += sample;
            
            // Advance phase
            enginePhase += 2.0f * PI * baseFreq / audioSpec.freq;
            if (enginePhase > 2.0f * PI) enginePhase -= 2.0f * PI;
        }
    }
    
    // Generate wind sound based on speed
    float windVol = sfxVolume * volume * (currentEnginePitch - 1.0f) * 0.15f;
    if (windVol > 0.01f) {
        for (int i = 0; i < sampleCount; i++) {
            // White noise filtered for wind effect
            float noise = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
            
            // Simple low-pass filter
            static float lastSample = 0.0f;
            float sample = lastSample * 0.9f + noise * 0.1f;
            lastSample = sample;
            
            sample *= windVol;
            
            buffer[i * 2] += sample;
            buffer[i * 2 + 1] += sample;
        }
    }
    
    // Music (simple ambient tone)
    if (musicPlaying) {
        float musicVol = musicVolume * volume * musicFadeVolume * 0.1f;
        for (int i = 0; i < sampleCount; i++) {
            // Generate ambient music tone
            static float musicPhase1 = 0.0f;
            static float musicPhase2 = 0.0f;
            static float musicPhase3 = 0.0f;
            
            float sample = 0.0f;
            sample += std::sin(musicPhase1) * 0.4f;
            sample += std::sin(musicPhase2) * 0.3f;
            sample += std::sin(musicPhase3) * 0.2f;
            
            sample *= musicVol;
            
            buffer[i * 2] += sample * 0.8f;
            buffer[i * 2 + 1] += sample * 0.8f;
            
            // Different frequencies for ambient feel
            float lfo = std::sin(musicPhase1 * 0.01f) * 0.1f;
            musicPhase1 += 2.0f * PI * (220.0f + lfo * 20.0f) / audioSpec.freq;
            musicPhase2 += 2.0f * PI * 277.18f / audioSpec.freq;  // C#
            musicPhase3 += 2.0f * PI * 329.63f / audioSpec.freq;  // E
            
            if (musicPhase1 > 2.0f * PI) musicPhase1 -= 2.0f * PI;
            if (musicPhase2 > 2.0f * PI) musicPhase2 -= 2.0f * PI;
            if (musicPhase3 > 2.0f * PI) musicPhase3 -= 2.0f * PI;
        }
    }
    
    // Clamp output
    for (int i = 0; i < sampleCount * 2; i++) {
        if (buffer[i] > 1.0f) buffer[i] = 1.0f;
        if (buffer[i] < -1.0f) buffer[i] = -1.0f;
    }
}

void AudioManager::update(float deltaTime) {
    // Handle music fade
    if (musicFadeVolume < 1.0f) {
        musicFadeVolume += deltaTime;
        if (musicFadeVolume > 1.0f) musicFadeVolume = 1.0f;
    }
}

void AudioManager::playSound(SoundEffect sound, float volume, bool loop) {
    // In a full implementation, this would trigger specific sound effects
    // For now, sound effects are synthesized in real-time
}

void AudioManager::stopSound(SoundEffect sound) {
    // Stop specific sound effect
}

void AudioManager::stopAllSounds() {
    // Stop all sound effects
}

void AudioManager::playMusic(MusicTrack track, bool loop) {
    currentTrack = track;
    musicPlaying = true;
    musicFadeVolume = 0.0f;  // Start fade in
}

void AudioManager::stopMusic() {
    musicPlaying = false;
}

void AudioManager::pauseMusic() {
    musicPlaying = false;
}

void AudioManager::resumeMusic() {
    musicPlaying = true;
}

void AudioManager::fadeOutMusic(float duration) {
    // Gradually reduce music volume
    musicFadeVolume = 0.0f;
}

void AudioManager::updateEngineSound(float throttle, float speed) {
    // Smooth transition for engine sound
    currentEngineThrottle += (throttle - currentEngineThrottle) * 0.1f;
    
    // Pitch increases with speed
    float targetPitch = 1.0f + speed / 100.0f;
    currentEnginePitch += (targetPitch - currentEnginePitch) * 0.1f;
}

void AudioManager::setMasterVolume(float volume) {
    masterVolume = std::max(0.0f, std::min(1.0f, volume));
}

void AudioManager::setMusicVolume(float volume) {
    musicVolume = std::max(0.0f, std::min(1.0f, volume));
}

void AudioManager::setSFXVolume(float volume) {
    sfxVolume = std::max(0.0f, std::min(1.0f, volume));
}

void AudioManager::setEngineVolume(float volume) {
    engineVolume = std::max(0.0f, std::min(1.0f, volume));
}

void AudioManager::toggleMute() {
    muted = !muted;
    if (muted) {
        previousMasterVolume = masterVolume;
    }
}

void AudioManager::generateTone(float* buffer, int sampleCount, float frequency, float volume) {
    static float phase = 0.0f;
    
    for (int i = 0; i < sampleCount; i++) {
        float sample = std::sin(phase) * volume;
        buffer[i * 2] += sample;      // Left
        buffer[i * 2 + 1] += sample;  // Right
        
        phase += 2.0f * PI * frequency / audioSpec.freq;
        if (phase > 2.0f * PI) phase -= 2.0f * PI;
    }
}
