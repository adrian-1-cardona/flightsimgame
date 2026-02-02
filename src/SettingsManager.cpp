#include "SettingsManager.h"
#include <fstream>
#include <iostream>
#include <sstream>

SettingsManager::SettingsManager() {
    resetToDefaults();
}

SettingsManager::~SettingsManager() {}

void SettingsManager::resetToDefaults() {
    settings.masterVolume = 0.8f;
    settings.musicVolume = 0.7f;
    settings.sfxVolume = 0.8f;
    settings.engineVolume = 0.9f;
    settings.fullscreen = false;
    settings.resolutionWidth = 1920;
    settings.resolutionHeight = 1080;
    settings.vsync = true;
    settings.mouseSensitivity = 1.0f;
    settings.controllerSensitivity = 1.0f;
    settings.invertYAxis = false;
    settings.showHUD = true;
    settings.showMinimap = true;
    settings.graphicsQuality = 2;  // High
}

bool SettingsManager::loadSettings(const std::string& filepath) {
    settingsPath = filepath;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "No settings file found, using defaults" << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream iss(line);
        std::string key;
        if (std::getline(iss, key, '=')) {
            std::string value;
            if (std::getline(iss, value)) {
                // Remove whitespace
                while (!key.empty() && (key.back() == ' ' || key.back() == '\t')) 
                    key.pop_back();
                while (!value.empty() && (value.front() == ' ' || value.front() == '\t')) 
                    value.erase(0, 1);
                
                // Parse settings
                if (key == "masterVolume") settings.masterVolume = std::stof(value);
                else if (key == "musicVolume") settings.musicVolume = std::stof(value);
                else if (key == "sfxVolume") settings.sfxVolume = std::stof(value);
                else if (key == "engineVolume") settings.engineVolume = std::stof(value);
                else if (key == "fullscreen") settings.fullscreen = (value == "true" || value == "1");
                else if (key == "resolutionWidth") settings.resolutionWidth = std::stoi(value);
                else if (key == "resolutionHeight") settings.resolutionHeight = std::stoi(value);
                else if (key == "vsync") settings.vsync = (value == "true" || value == "1");
                else if (key == "mouseSensitivity") settings.mouseSensitivity = std::stof(value);
                else if (key == "controllerSensitivity") settings.controllerSensitivity = std::stof(value);
                else if (key == "invertYAxis") settings.invertYAxis = (value == "true" || value == "1");
                else if (key == "showHUD") settings.showHUD = (value == "true" || value == "1");
                else if (key == "showMinimap") settings.showMinimap = (value == "true" || value == "1");
                else if (key == "graphicsQuality") settings.graphicsQuality = std::stoi(value);
            }
        }
    }
    
    file.close();
    std::cout << "Settings loaded from " << filepath << std::endl;
    return true;
}

bool SettingsManager::saveSettings(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to save settings to " << filepath << std::endl;
        return false;
    }
    
    file << "# Flight Simulator Pro Settings\n";
    file << "# Generated automatically\n\n";
    
    file << "# Audio\n";
    file << "masterVolume = " << settings.masterVolume << "\n";
    file << "musicVolume = " << settings.musicVolume << "\n";
    file << "sfxVolume = " << settings.sfxVolume << "\n";
    file << "engineVolume = " << settings.engineVolume << "\n\n";
    
    file << "# Display\n";
    file << "fullscreen = " << (settings.fullscreen ? "true" : "false") << "\n";
    file << "resolutionWidth = " << settings.resolutionWidth << "\n";
    file << "resolutionHeight = " << settings.resolutionHeight << "\n";
    file << "vsync = " << (settings.vsync ? "true" : "false") << "\n";
    file << "graphicsQuality = " << settings.graphicsQuality << "\n\n";
    
    file << "# Controls\n";
    file << "mouseSensitivity = " << settings.mouseSensitivity << "\n";
    file << "controllerSensitivity = " << settings.controllerSensitivity << "\n";
    file << "invertYAxis = " << (settings.invertYAxis ? "true" : "false") << "\n\n";
    
    file << "# HUD\n";
    file << "showHUD = " << (settings.showHUD ? "true" : "false") << "\n";
    file << "showMinimap = " << (settings.showMinimap ? "true" : "false") << "\n";
    
    file.close();
    std::cout << "Settings saved to " << filepath << std::endl;
    return true;
}
