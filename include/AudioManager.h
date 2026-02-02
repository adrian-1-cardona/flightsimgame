#pragma once

#include "Types.h"
#include <SDL2/SDL.h>
#include <map>
#include <string>

enum class SoundEffect {
    ENGINE_IDLE,
    ENGINE_FULL,
    WIND,
    LANDING_GEAR,
    FLAPS,
    STALL_WARNING,
    TOUCHDOWN,
    MENU_SELECT,
    MENU_MOVE,
    BUTTON_CLICK
};

enum class MusicTrack {
    MENU_THEME,
    FLIGHT_AMBIENT,
    LOADING_THEME
};

class AudioManager {
public:
    AudioManager();
    ~AudioManager();
    
    bool initialize();
    void shutdown();
    void update(float deltaTime);
    
    // Sound effects
    void playSound(SoundEffect sound, float volume = 1.0f, bool loop = false);
    void stopSound(SoundEffect sound);
    void stopAllSounds();
    
    // Music
    void playMusic(MusicTrack track, bool loop = true);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    void fadeOutMusic(float duration);
    
    // Engine sound (special handling for continuous adjustment)
    void updateEngineSound(float throttle, float speed);
    
    // Volume controls
    void setMasterVolume(float volume);
    void setMusicVolume(float volume);
    void setSFXVolume(float volume);
    void setEngineVolume(float volume);
    
    float getMasterVolume() const { return masterVolume; }
    float getMusicVolume() const { return musicVolume; }
    float getSFXVolume() const { return sfxVolume; }
    float getEngineVolume() const { return engineVolume; }
    
    // Mute
    void toggleMute();
    bool isMuted() const { return muted; }
    
private:
    void generateTone(float* buffer, int sampleCount, float frequency, float volume);
    void mixAudio(Uint8* stream, int len);
    static void audioCallback(void* userdata, Uint8* stream, int len);
    
    SDL_AudioDeviceID audioDevice = 0;
    SDL_AudioSpec audioSpec;
    
    // Volume levels (0.0 - 1.0)
    float masterVolume = 0.8f;
    float musicVolume = 0.7f;
    float sfxVolume = 0.8f;
    float engineVolume = 0.9f;
    bool muted = false;
    float previousMasterVolume = 0.8f;
    
    // Engine sound state
    float currentEngineThrottle = 0.0f;
    float currentEnginePitch = 1.0f;
    
    // Music state
    MusicTrack currentTrack = MusicTrack::MENU_THEME;
    bool musicPlaying = false;
    float musicFadeVolume = 1.0f;
    
    // Sound synthesis
    float enginePhase = 0.0f;
    float windPhase = 0.0f;
    bool isInitialized = false;
};
