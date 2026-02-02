#pragma once

#include "Types.h"
#include <vector>
#include <string>
#include <functional>

class Game;
class Renderer;
class InputManager;

class MenuSystem {
public:
    MenuSystem(Game* game);
    ~MenuSystem();
    
    void update(float deltaTime);
    void render(Renderer* renderer);
    
    // Menu navigation
    void navigateUp();
    void navigateDown();
    void navigateLeft();
    void navigateRight();
    void select();
    void back();
    void handleMouse(InputManager* input, int screenWidth, int screenHeight);
    
    // Menu state
    void showMainMenu();
    void showAircraftSelect();
    void showPauseMenu();
    void showSettingsMenu();
    void showControlsSettings();
    void showVolumeSettings();
    
private:
    void setupMainMenu();
    void setupAircraftSelectMenu();
    void setupPauseMenu();
    void setupSettingsMenu();
    void setupControlsMenu();
    void setupVolumeMenu();
    
    void renderMainMenu(Renderer* renderer);
    void renderAircraftSelect(Renderer* renderer);
    void renderPauseMenu(Renderer* renderer);
    void renderSettingsMenu(Renderer* renderer);
    void renderControlsSettings(Renderer* renderer);
    void renderVolumeSettings(Renderer* renderer);
    void renderAircraftPreview(Renderer* renderer, AircraftType type);
    
    Game* game;
    
    // Current menu state
    enum class MenuState {
        MAIN,
        AIRCRAFT_SELECT,
        PAUSE,
        SETTINGS,
        CONTROLS,
        VOLUME
    };
    MenuState currentMenu = MenuState::MAIN;
    
    // Menu items
    std::vector<MenuItem> currentMenuItems;
    int selectedIndex = 0;
    
    // Aircraft selection
    int selectedAircraftIndex = 0;
    std::vector<AircraftType> availableAircraft;
    
    // Animation
    float animationTime = 0.0f;
    float transitionAlpha = 1.0f;
    
    // Settings being edited
    int editingControlIndex = -1;
    bool waitingForInput = false;
    
    // Menu item positions for mouse input (updated during render)
    struct MenuItemRect {
        float x, y, width, height;
    };
    std::vector<MenuItemRect> menuItemRects;
};
