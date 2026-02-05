#pragma once

#include "Types.h"
#include <string>
#include <algorithm>

class SettingsManager {
public:
    SettingsManager();
    ~SettingsManager();
    
    bool loadSettings(const std::string& filepath);
    bool saveSettings(const std::string& filepath);
    void resetToDefaults();
    
    // Getters
    GameSettings& getSettings() { return settings; }
    const GameSettings& getSettings() const { return settings; }
    
    // Audio settings
    float getMasterVolume() const { return settings.masterVolume; }
    float getMusicVolume() const { return settings.musicVolume; }
    float getSFXVolume() const { return settings.sfxVolume; }
    float getEngineVolume() const { return settings.engineVolume; }
    
    void setMasterVolume(float vol) { settings.masterVolume = std::clamp(vol, 0.0f, 1.0f); }
    void setMusicVolume(float vol) { settings.musicVolume = std::clamp(vol, 0.0f, 1.0f); }
    void setSFXVolume(float vol) { settings.sfxVolume = std::clamp(vol, 0.0f, 1.0f); }
    void setEngineVolume(float vol) { settings.engineVolume = std::clamp(vol, 0.0f, 1.0f); }
    
    // Display settings
    bool isFullscreen() const { return settings.fullscreen; }
    int getResolutionWidth() const { return settings.resolutionWidth; }
    int getResolutionHeight() const { return settings.resolutionHeight; }
    bool isVSyncEnabled() const { return settings.vsync; }
    int getGraphicsQuality() const { return settings.graphicsQuality; }
    
    void setFullscreen(bool fs) { settings.fullscreen = fs; }
    void setResolution(int w, int h) { settings.resolutionWidth = w; settings.resolutionHeight = h; }
    void setVSync(bool vs) { settings.vsync = vs; }
    void setGraphicsQuality(int q) { settings.graphicsQuality = std::clamp(q, 0, 3); }
    
    // Control settings
    float getMouseSensitivity() const { return settings.mouseSensitivity; }
    float getControllerSensitivity() const { return settings.controllerSensitivity; }
    bool isYAxisInverted() const { return settings.invertYAxis; }
    
    void setMouseSensitivity(float sens) { settings.mouseSensitivity = std::clamp(sens, 0.1f, 3.0f); }
    void setControllerSensitivity(float sens) { settings.controllerSensitivity = std::clamp(sens, 0.1f, 3.0f); }
    void setInvertYAxis(bool invert) { settings.invertYAxis = invert; }
    
    // HUD settings
    bool isHUDEnabled() const { return settings.showHUD; }
    bool isMinimapEnabled() const { return settings.showMinimap; }
    
    void setHUDEnabled(bool enabled) { settings.showHUD = enabled; }
    void setMinimapEnabled(bool enabled) { settings.showMinimap = enabled; }
    
private:
    GameSettings settings;
    std::string settingsPath;
};
