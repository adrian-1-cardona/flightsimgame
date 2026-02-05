#include "Game.h"
#include "Renderer.h"
#include "Aircraft.h"
#include "InputManager.h"
#include "MenuSystem.h"
#include "AudioManager.h"
#include "SettingsManager.h"
#include "Terrain.h"
#include "Sky.h"
#include "LoadingScreen.h"
#include "Camera.h"
#include "Physics.h"

#include <iostream>
#include <thread>
#include <chrono>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

Game::Game() {
}

Game::~Game() {
    shutdown();
}

bool Game::initialize() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    
    // Create window
    window = SDL_CreateWindow(
        windowTitle.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Mouse mode will be set based on game state (disabled for menus, enabled for gameplay)
    
    // Create OpenGL context
    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "OpenGL context creation failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Enable VSync
    SDL_GL_SetSwapInterval(1);
    
    // Initialize subsystems
    renderer = std::make_unique<Renderer>();
    if (!renderer->initialize(window, windowWidth, windowHeight)) {
        std::cerr << "Renderer initialization failed!" << std::endl;
        return false;
    }
    
    
    std::cout << "Initializing settings manager..." << std::endl;
    settingsManager = std::make_unique<SettingsManager>();
    settingsManager->loadSettings("settings.cfg");
    
    audioManager = std::make_unique<AudioManager>();
    if (!audioManager->initialize()) {
        std::cerr << "Warning: Audio initialization failed, continuing without sound" << std::endl;
    }
    
    inputManager = std::make_unique<InputManager>();
    if (!inputManager->initialize()) {
        std::cerr << "Input manager initialization failed!" << std::endl;
        return false;
    }
    
    // Scan for controllers
    inputManager->scanForControllers();
    
    std::cout << "Initializing menu system..." << std::endl;
    menuSystem = std::make_unique<MenuSystem>(this);
    
    terrain = std::make_unique<Terrain>();
    terrain->generate(32, 10.0f);  // Smaller chunks for better performance
    
    sky = std::make_unique<Sky>();
    sky->setTimeOfDay(12.0f);
    
    loadingScreen = std::make_unique<LoadingScreen>();
    
    camera = std::make_unique<Camera>();
    camera->setMode(CameraMode::CHASE);
    
    physics = std::make_unique<Physics>();
    
    // Create default aircraft
    selectAircraft(AircraftType::BOEING_737);
    
    isRunning = true;
    lastFrameTime = SDL_GetPerformanceCounter();
    
    return true;
}

void Game::run() {
    // Show main menu
    setState(GameState::MAIN_MENU);
    
    while (isRunning) {
        // Calculate delta time
        Uint64 currentTime = SDL_GetPerformanceCounter();
        deltaTime = (float)(currentTime - lastFrameTime) / SDL_GetPerformanceFrequency();
        lastFrameTime = currentTime;
        
        // Cap delta time to prevent physics explosions
        if (deltaTime > 0.1f) deltaTime = 0.1f;
        
        processInput();
        update(deltaTime);
        render();
        
        // Clear old input state at the END of frame (after being used)
        inputManager->clearMouseClicksThisFrame();
        inputManager->clearKeyPressesThisFrame();
        
        // Frame limiting (optional, VSync should handle this)
        SDL_GL_SwapWindow(window);
    }
}

void Game::shutdown() {
    std::cout << "Shutting down game..." << std::endl;
    
    // Save settings
    if (settingsManager) {
        settingsManager->saveSettings("settings.cfg");
    }
    
    // Clean up subsystems
    if (audioManager) audioManager->shutdown();
    if (inputManager) inputManager->shutdown();
    if (renderer) renderer->shutdown();
    
    // Destroy window and OpenGL context
    if (glContext) {
        SDL_GL_DeleteContext(glContext);
        glContext = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    SDL_Quit();
    std::cout << "Shutdown complete." << std::endl;
}

void Game::processInput() {
    SDL_Event event;
    
    // Poll and process all SDL events first (this will set mouse clicks if they occur)
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                isRunning = false;
                break;
                
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    if (currentState == GameState::PLAYING) {
                        togglePause();
                    } else if (currentState == GameState::PAUSED) {
                        togglePause();
                    } else if (currentState != GameState::MAIN_MENU) {
                        setState(GameState::MAIN_MENU);
                    }
                }
                if (event.key.keysym.sym == SDLK_F11) {
                    // Toggle fullscreen
                    Uint32 flags = SDL_GetWindowFlags(window);
                    if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
                        SDL_SetWindowFullscreen(window, 0);
                    } else {
                        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                    }
                }
                break;
                
            case SDL_CONTROLLERDEVICEADDED:
                inputManager->scanForControllers();
                std::cout << "Controller connected!" << std::endl;
                break;
                
            case SDL_CONTROLLERDEVICEREMOVED:
                inputManager->scanForControllers();
                std::cout << "Controller disconnected!" << std::endl;
                break;
                
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    windowWidth = event.window.data1;
                    windowHeight = event.window.data2;
                    renderer->setViewport(0, 0, windowWidth, windowHeight);
                }
                break;
        }
        
        inputManager->processEvent(event);
    }
    
    inputManager->update();
    
    // Clear clicks AFTER they've been used by handleMouse (done in update() method)
    // This will be called at the end of the game loop
}

void Game::update(float deltaTime) {
    switch (currentState) {
        case GameState::LOADING:
            loadingScreen->update(deltaTime);
            break;
            
        case GameState::MAIN_MENU:
        case GameState::AIRCRAFT_SELECT:
        case GameState::PAUSED:
        case GameState::SETTINGS:
        case GameState::CONTROLS_SETTINGS:
            menuSystem->update(deltaTime);
            
            // Handle mouse input for menus
            menuSystem->handleMouse(inputManager.get(), windowWidth, windowHeight);
            
            // Handle menu navigation using direct keyboard check
            if (inputManager->isKeyJustPressed(SDL_SCANCODE_UP) || inputManager->isKeyJustPressed(SDL_SCANCODE_W)) {
                menuSystem->navigateUp();
                audioManager->playSound(SoundEffect::MENU_MOVE);
            }
            if (inputManager->isKeyJustPressed(SDL_SCANCODE_DOWN) || inputManager->isKeyJustPressed(SDL_SCANCODE_S)) {
                menuSystem->navigateDown();
                audioManager->playSound(SoundEffect::MENU_MOVE);
            }
            if (inputManager->isKeyJustPressed(SDL_SCANCODE_LEFT) || inputManager->isKeyJustPressed(SDL_SCANCODE_A)) {
                menuSystem->navigateLeft();
            }
            if (inputManager->isKeyJustPressed(SDL_SCANCODE_RIGHT) || inputManager->isKeyJustPressed(SDL_SCANCODE_D)) {
                menuSystem->navigateRight();
            }
            if (inputManager->isKeyJustPressed(SDL_SCANCODE_RETURN) || inputManager->isKeyJustPressed(SDL_SCANCODE_SPACE)) {
                menuSystem->select();
                audioManager->playSound(SoundEffect::MENU_SELECT);
            }
            if (inputManager->isKeyJustPressed(SDL_SCANCODE_ESCAPE) || inputManager->isKeyJustPressed(SDL_SCANCODE_BACKSPACE)) {
                menuSystem->back();
            }
            break;
            
        case GameState::PLAYING: {
            if (currentAircraft) {
                // Apply flight controls from keyboard/controller
                float throttleChange = inputManager->getThrottleAxis() * deltaTime * 0.5f;
                currentAircraft->setThrottle(currentAircraft->getThrottle() + throttleChange);
                currentAircraft->setPitch(inputManager->getPitchAxis());
                currentAircraft->setRoll(inputManager->getRollAxis());
                currentAircraft->setYaw(inputManager->getYawAxis());
                
                // Handle discrete inputs using reliable "pressed this frame" checks
                if (inputManager->wasKeyPressedThisFrame(SDL_SCANCODE_G)) {
                    currentAircraft->toggleLandingGear();
                    audioManager->playSound(SoundEffect::LANDING_GEAR);
                }
                if (inputManager->wasKeyPressedThisFrame(SDL_SCANCODE_F)) {
                    currentAircraft->adjustFlaps(1);
                    audioManager->playSound(SoundEffect::FLAPS);
                }
                if (inputManager->wasKeyPressedThisFrame(SDL_SCANCODE_V)) {
                    currentAircraft->adjustFlaps(-1);
                    audioManager->playSound(SoundEffect::FLAPS);
                }
                if (inputManager->isKeyDown(SDL_SCANCODE_B)) {
                    currentAircraft->setBrake(true);
                } else {
                    currentAircraft->setBrake(false);
                }
                
                // Camera toggle
                if (inputManager->wasKeyPressedThisFrame(SDL_SCANCODE_C)) {
                    camera->cycleMode();
                }
                
                // Pause
                if (inputManager->wasKeyPressedThisFrame(SDL_SCANCODE_ESCAPE) || 
                    inputManager->wasKeyPressedThisFrame(SDL_SCANCODE_P)) {
                    togglePause();
                }
                
                // Update physics
                physics->update(deltaTime, currentAircraft.get(), terrain.get());
                
                // Update aircraft
                currentAircraft->update(deltaTime);
                
                // Update audio
                audioManager->updateEngineSound(currentAircraft->getThrottle(), 
                                                currentAircraft->getSpeed());
                
                // Check for stall warning
                if (currentAircraft->isStalling()) {
                    audioManager->playSound(SoundEffect::STALL_WARNING, 0.8f);
                }
            }
            
            // Update camera
            camera->update(deltaTime, currentAircraft.get());
            
            // Update sky
            sky->update(deltaTime);
            
            // Update terrain with player position for infinite world generation
            terrain->update(deltaTime, currentAircraft->getPosition());
            
            // Update audio
            audioManager->update(deltaTime);
            break;
        }
            
        case GameState::QUIT:
            isRunning = false;
            break;
    }
}

void Game::render() {
    renderer->beginFrame();
    
    switch (currentState) {
        case GameState::LOADING:
            loadingScreen->render(renderer.get());
            break;
            
        case GameState::MAIN_MENU:
        case GameState::AIRCRAFT_SELECT:
        case GameState::SETTINGS:
        case GameState::CONTROLS_SETTINGS:
            menuSystem->render(renderer.get());
            break;
            
        case GameState::PLAYING:
        case GameState::PAUSED:
            // Render 3D world
            if (camera && currentAircraft) {
                // Setup camera
                float aspect = (float)windowWidth / (float)windowHeight;
                renderer->setProjectionMatrix(camera->getFOV(), aspect, 
                                             camera->getNearPlane(), camera->getFarPlane());
                renderer->setViewMatrix(camera->getPosition(), camera->getTarget(), camera->getUp());
                
                // Render sky
                renderer->renderSky(sky.get(), camera.get());
                
                // Render terrain
                renderer->renderTerrain(terrain.get(), camera.get());
                
                // Render aircraft
                renderer->renderAircraft(currentAircraft.get(), camera.get());
                
                // Render HUD
                if (settingsManager->isHUDEnabled()) {
                    renderer->renderHUD(currentAircraft.get());
                }
                
                // Render minimap
                if (settingsManager->isMinimapEnabled()) {
                    renderer->renderMinimap(currentAircraft.get(), terrain.get());
                }
            }
            
            // Render pause menu overlay
            if (currentState == GameState::PAUSED) {
                menuSystem->render(renderer.get());
            }
            break;
            
        default:
            break;
    }
    
    renderer->endFrame();
}

void Game::loadResources() {
    std::cout << "Loading resources..." << std::endl;
    
    loadingScreen->setLoadingText("Initializing graphics...");
    loadingScreen->setProgress(0.1f);
    render();
    SDL_GL_SwapWindow(window);
    SDL_Delay(100);
    
    loadingScreen->setLoadingText("Loading terrain data...");
    loadingScreen->setProgress(0.3f);
    render();
    SDL_GL_SwapWindow(window);
    SDL_Delay(100);
    
    loadingScreen->setLoadingText("Loading aircraft models...");
    loadingScreen->setProgress(0.5f);
    render();
    SDL_GL_SwapWindow(window);
    SDL_Delay(100);
    
    loadingScreen->setLoadingText("Initializing physics engine...");
    loadingScreen->setProgress(0.7f);
    render();
    SDL_GL_SwapWindow(window);
    SDL_Delay(100);
    
    loadingScreen->setLoadingText("Loading audio...");
    loadingScreen->setProgress(0.85f);
    render();
    SDL_GL_SwapWindow(window);
    SDL_Delay(100);
    
    loadingScreen->setLoadingText("Ready for takeoff!");
    loadingScreen->setProgress(1.0f);
    render();
    SDL_GL_SwapWindow(window);
    SDL_Delay(500);
    
    std::cout << "Resources loaded!" << std::endl;
}

void Game::setState(GameState newState) {
    currentState = newState;
    
    switch (newState) {
        case GameState::MAIN_MENU:
            menuSystem->showMainMenu();
            audioManager->playMusic(MusicTrack::MENU_THEME);
            // Disable relative mouse mode for menu navigation
            SDL_SetRelativeMouseMode(SDL_FALSE);
            SDL_SetWindowGrab(window, SDL_FALSE);
            break;
        case GameState::AIRCRAFT_SELECT:
            menuSystem->showAircraftSelect();
            // Disable relative mouse mode for menu navigation
            SDL_SetRelativeMouseMode(SDL_FALSE);
            SDL_SetWindowGrab(window, SDL_FALSE);
            break;
        case GameState::PAUSED:
            menuSystem->showPauseMenu();
            // Disable relative mouse mode for menu navigation
            SDL_SetRelativeMouseMode(SDL_FALSE);
            SDL_SetWindowGrab(window, SDL_FALSE);
            break;
        case GameState::SETTINGS:
            menuSystem->showSettingsMenu();
            // Disable relative mouse mode for menu navigation
            SDL_SetRelativeMouseMode(SDL_FALSE);
            SDL_SetWindowGrab(window, SDL_FALSE);
            break;
        case GameState::CONTROLS_SETTINGS:
            menuSystem->showControlsSettings();
            // Disable relative mouse mode for menu navigation
            SDL_SetRelativeMouseMode(SDL_FALSE);
            SDL_SetWindowGrab(window, SDL_FALSE);
            break;
        case GameState::PLAYING:
            audioManager->playMusic(MusicTrack::FLIGHT_AMBIENT);
            // Enable relative mouse mode for flight control (for future camera control)
            SDL_SetRelativeMouseMode(SDL_TRUE);
            SDL_SetWindowGrab(window, SDL_TRUE);
            break;
        default:
            break;
    }
}

void Game::selectAircraft(AircraftType type) {
    selectedAircraftType = type;
    currentAircraft = std::make_unique<Aircraft>(type);
    currentAircraft->reset();
    
    std::cout << "Selected aircraft: " << currentAircraft->getSpecs().name << std::endl;
    std::cout << "Ready to fly! Use WASD/Arrows for pitch/roll, Z/X for yaw, Q/E for throttle." << std::endl;
}

void Game::togglePause() {
    if (currentState == GameState::PLAYING) {
        setState(GameState::PAUSED);
        audioManager->pauseMusic();
    } else if (currentState == GameState::PAUSED) {
        setState(GameState::PLAYING);
        audioManager->resumeMusic();
    }
}
