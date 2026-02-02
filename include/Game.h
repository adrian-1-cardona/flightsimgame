#pragma once

#include "Types.h"
#include <SDL2/SDL.h>
#include <memory>

// Forward declarations
class Renderer;
class Aircraft;
class InputManager;
class MenuSystem;
class AudioManager;
class SettingsManager;
class Terrain;
class Sky;
class LoadingScreen;
class Camera;
class Physics;

class Game {
public:
    Game();
    ~Game();
    
    bool initialize();
    void run();
    void shutdown();
    
    // State management
    void setState(GameState newState);
    GameState getState() const { return currentState; }
    
    // Getters for subsystems
    Renderer* getRenderer() { return renderer.get(); }
    InputManager* getInputManager() { return inputManager.get(); }
    AudioManager* getAudioManager() { return audioManager.get(); }
    SettingsManager* getSettingsManager() { return settingsManager.get(); }
    Camera* getCamera() { return camera.get(); }
    
    // Aircraft management
    void selectAircraft(AircraftType type);
    Aircraft* getCurrentAircraft() { return currentAircraft.get(); }
    
    // Game control
    void togglePause();
    bool isPaused() const { return currentState == GameState::PAUSED; }
    
private:
    void processInput();
    void update(float deltaTime);
    void render();
    void loadResources();
    
    // Core systems
    SDL_Window* window = nullptr;
    SDL_GLContext glContext = nullptr;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<InputManager> inputManager;
    std::unique_ptr<MenuSystem> menuSystem;
    std::unique_ptr<AudioManager> audioManager;
    std::unique_ptr<SettingsManager> settingsManager;
    std::unique_ptr<Terrain> terrain;
    std::unique_ptr<Sky> sky;
    std::unique_ptr<LoadingScreen> loadingScreen;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Physics> physics;
    
    // Current aircraft
    std::unique_ptr<Aircraft> currentAircraft;
    AircraftType selectedAircraftType = AircraftType::BOEING_737;
    
    // Game state
    GameState currentState = GameState::LOADING;
    GameState previousState = GameState::LOADING;
    bool isRunning = false;
    
    // Timing
    Uint64 lastFrameTime = 0;
    float deltaTime = 0.0f;
    float loadingProgress = 0.0f;
    
    // Window properties
    int windowWidth = 1920;
    int windowHeight = 1080;
    std::string windowTitle = "Flight Simulator Pro";
};
