#include "MenuSystem.h"
#include "Game.h"
#include "Renderer.h"
#include "InputManager.h"
#include "AudioManager.h"
#include "SettingsManager.h"
#include "Aircraft.h"

#include <iostream>
#include <cmath>

MenuSystem::MenuSystem(Game* g) : game(g) {
    // Initialize available aircraft
    availableAircraft = {
        AircraftType::BOEING_737,
        AircraftType::F16_FIGHTER,
        AircraftType::F22_RAPTOR,
        AircraftType::CESSNA_172,
        AircraftType::A320_AIRBUS
    };
    
    setupMainMenu();
}

MenuSystem::~MenuSystem() {}

void MenuSystem::update(float deltaTime) {
    animationTime += deltaTime;
    
    // Transition animation
    if (transitionAlpha < 1.0f) {
        transitionAlpha += deltaTime * 3.0f;
        if (transitionAlpha > 1.0f) transitionAlpha = 1.0f;
    }
}

void MenuSystem::handleMouse(InputManager* input, int screenWidth, int screenHeight) {
    int mouseX = input->getMouseX();
    int mouseY = input->getMouseY();
    
    // Special handling for aircraft select - check for clickable aircraft
    if (currentMenu == MenuState::AIRCRAFT_SELECT) {
        // Check for aircraft boxes (first 3 rects) and FLY button (last rect)
        if (menuItemRects.size() >= 4) {
            // Left aircraft (previous)
            if (mouseX >= menuItemRects[0].x && mouseX <= menuItemRects[0].x + menuItemRects[0].width &&
                mouseY >= menuItemRects[0].y && mouseY <= menuItemRects[0].y + menuItemRects[0].height) {
                if (input->wasMouseButtonClickedThisFrame(SDL_BUTTON_LEFT)) {
                    navigateLeft();
                    return;
                }
            }
            
            // Middle aircraft (currently selected)
            if (menuItemRects.size() > 1 &&
                mouseX >= menuItemRects[1].x && mouseX <= menuItemRects[1].x + menuItemRects[1].width &&
                mouseY >= menuItemRects[1].y && mouseY <= menuItemRects[1].y + menuItemRects[1].height) {
                // Just highlight, don't change
                return;
            }
            
            // Right aircraft (next)
            if (menuItemRects.size() > 2 &&
                mouseX >= menuItemRects[2].x && mouseX <= menuItemRects[2].x + menuItemRects[2].width &&
                mouseY >= menuItemRects[2].y && mouseY <= menuItemRects[2].y + menuItemRects[2].height) {
                if (input->wasMouseButtonClickedThisFrame(SDL_BUTTON_LEFT)) {
                    navigateRight();
                    return;
                }
            }
            
            // FLY button (last rect)
            if (menuItemRects.size() > 3 &&
                mouseX >= menuItemRects[3].x && mouseX <= menuItemRects[3].x + menuItemRects[3].width &&
                mouseY >= menuItemRects[3].y && mouseY <= menuItemRects[3].y + menuItemRects[3].height) {
                if (input->wasMouseButtonClickedThisFrame(SDL_BUTTON_LEFT)) {
                    selectedIndex = 0; // Select FLY button
                    select();
                    return;
                }
            }
        }
        return;
    }
    
    // Check if mouse is over any menu item
    for (int i = 0; i < (int)menuItemRects.size() && i < (int)currentMenuItems.size(); i++) {
        const MenuItemRect& rect = menuItemRects[i];
        if (mouseX >= rect.x && mouseX <= rect.x + rect.width &&
            mouseY >= rect.y && mouseY <= rect.y + rect.height) {
            // Mouse is over this item - highlight it
            if (selectedIndex != i) {
                selectedIndex = i;
            }
            
            // Check for click using the new reliable method
            if (input->wasMouseButtonClickedThisFrame(SDL_BUTTON_LEFT)) {
                select();
            }
            break;
        }
    }
}

void MenuSystem::render(Renderer* renderer) {
    switch (currentMenu) {
        case MenuState::MAIN:
            renderMainMenu(renderer);
            break;
        case MenuState::AIRCRAFT_SELECT:
            renderAircraftSelect(renderer);
            break;
        case MenuState::PAUSE:
            renderPauseMenu(renderer);
            break;
        case MenuState::SETTINGS:
            renderSettingsMenu(renderer);
            break;
        case MenuState::CONTROLS:
            renderControlsSettings(renderer);
            break;
        case MenuState::VOLUME:
            renderVolumeSettings(renderer);
            break;
    }
}

void MenuSystem::navigateUp() {
    if (currentMenuItems.empty()) return;
    
    selectedIndex--;
    if (selectedIndex < 0) {
        selectedIndex = static_cast<int>(currentMenuItems.size()) - 1;
    }
    
    // Skip disabled items
    int attempts = 0;
    while (!currentMenuItems[selectedIndex].enabled && attempts < (int)currentMenuItems.size()) {
        selectedIndex--;
        if (selectedIndex < 0) selectedIndex = static_cast<int>(currentMenuItems.size()) - 1;
        attempts++;
    }
}

void MenuSystem::navigateDown() {
    if (currentMenuItems.empty()) return;
    
    selectedIndex++;
    if (selectedIndex >= (int)currentMenuItems.size()) {
        selectedIndex = 0;
    }
    
    // Skip disabled items
    int attempts = 0;
    while (!currentMenuItems[selectedIndex].enabled && attempts < (int)currentMenuItems.size()) {
        selectedIndex++;
        if (selectedIndex >= (int)currentMenuItems.size()) selectedIndex = 0;
        attempts++;
    }
}

void MenuSystem::navigateLeft() {
    if (currentMenu == MenuState::AIRCRAFT_SELECT) {
        selectedAircraftIndex--;
        if (selectedAircraftIndex < 0) {
            selectedAircraftIndex = static_cast<int>(availableAircraft.size()) - 1;
        }
        game->getAudioManager()->playSound(SoundEffect::MENU_MOVE);
    } else if (currentMenu == MenuState::VOLUME) {
        // Decrease volume
        updateVolumeSettings(-0.1f);
    }
}

void MenuSystem::navigateRight() {
    if (currentMenu == MenuState::AIRCRAFT_SELECT) {
        selectedAircraftIndex++;
        if (selectedAircraftIndex >= (int)availableAircraft.size()) {
            selectedAircraftIndex = 0;
        }
        game->getAudioManager()->playSound(SoundEffect::MENU_MOVE);
    } else if (currentMenu == MenuState::VOLUME) {
        // Increase volume
        updateVolumeSettings(0.1f);
    }
}

void MenuSystem::select() {
    if (waitingForInput) {
        // Handle control remapping
        waitingForInput = false;
        editingControlIndex = -1;
        return;
    }
    
    if (currentMenuItems.empty()) return;
    if (selectedIndex < 0 || selectedIndex >= (int)currentMenuItems.size()) return;
    if (!currentMenuItems[selectedIndex].enabled) return;
    
    if (currentMenuItems[selectedIndex].onSelect) {
        currentMenuItems[selectedIndex].onSelect();
    }
}

void MenuSystem::back() {
    if (waitingForInput) {
        waitingForInput = false;
        editingControlIndex = -1;
        return;
    }
    
    switch (currentMenu) {
        case MenuState::MAIN:
            // Exit game?
            break;
        case MenuState::AIRCRAFT_SELECT:
            showMainMenu();
            break;
        case MenuState::PAUSE:
            game->togglePause();  // Resume the game
            break;
        case MenuState::SETTINGS:
            if (game->getState() == GameState::PAUSED) {
                showPauseMenu();
            } else {
                showMainMenu();
            }
            break;
        case MenuState::CONTROLS:
        case MenuState::VOLUME:
            showSettingsMenu();
            break;
    }
}

void MenuSystem::setupMainMenu() {
    currentMenuItems.clear();
    selectedIndex = 0;
    
    MenuItem playItem;
    playItem.text = "SELECT AIRCRAFT";
    playItem.onSelect = [this]() {
        showAircraftSelect();
    };
    currentMenuItems.push_back(playItem);
    
    MenuItem settingsItem;
    settingsItem.text = "SETTINGS";
    settingsItem.onSelect = [this]() {
        showSettingsMenu();
    };
    currentMenuItems.push_back(settingsItem);
    
    MenuItem quitItem;
    quitItem.text = "QUIT";
    quitItem.onSelect = [this]() {
        game->setState(GameState::QUIT);
    };
    currentMenuItems.push_back(quitItem);
}

void MenuSystem::setupAircraftSelectMenu() {
    currentMenuItems.clear();
    selectedIndex = 0;
    selectedAircraftIndex = 0;
    
    MenuItem flyItem;
    flyItem.text = "FLY";
    flyItem.onSelect = [this]() {
        game->selectAircraft(availableAircraft[selectedAircraftIndex]);
        game->setState(GameState::PLAYING);
    };
    currentMenuItems.push_back(flyItem);
    
    MenuItem backItem;
    backItem.text = "BACK";
    backItem.onSelect = [this]() {
        showMainMenu();
    };
    currentMenuItems.push_back(backItem);
}

void MenuSystem::setupPauseMenu() {
    currentMenuItems.clear();
    selectedIndex = 0;
    
    MenuItem resumeItem;
    resumeItem.text = "RESUME";
    resumeItem.onSelect = [this]() {
        game->togglePause();
    };
    currentMenuItems.push_back(resumeItem);
    
    MenuItem settingsItem;
    settingsItem.text = "SETTINGS";
    settingsItem.onSelect = [this]() {
        showSettingsMenu();
    };
    currentMenuItems.push_back(settingsItem);
    
    MenuItem volumeItem;
    volumeItem.text = "VOLUME";
    volumeItem.onSelect = [this]() {
        showVolumeSettings();
    };
    currentMenuItems.push_back(volumeItem);
    
    MenuItem controlsItem;
    controlsItem.text = "CONTROLS";
    controlsItem.onSelect = [this]() {
        showControlsSettings();
    };
    currentMenuItems.push_back(controlsItem);
    
    MenuItem mainMenuItem;
    mainMenuItem.text = "MAIN MENU";
    mainMenuItem.onSelect = [this]() {
        game->setState(GameState::MAIN_MENU);
    };
    currentMenuItems.push_back(mainMenuItem);
}

void MenuSystem::setupSettingsMenu() {
    currentMenuItems.clear();
    selectedIndex = 0;
    
    MenuItem volumeItem;
    volumeItem.text = "VOLUME";
    volumeItem.onSelect = [this]() {
        showVolumeSettings();
    };
    currentMenuItems.push_back(volumeItem);
    
    MenuItem controlsItem;
    controlsItem.text = "CONTROLS";
    controlsItem.onSelect = [this]() {
        showControlsSettings();
    };
    currentMenuItems.push_back(controlsItem);
    
    auto settings = game->getSettingsManager();
    
    MenuItem invertYItem;
    invertYItem.text = settings->isYAxisInverted() ? "INVERT Y: ON" : "INVERT Y: OFF";
    invertYItem.onSelect = [this]() {
        auto s = game->getSettingsManager();
        s->setInvertYAxis(!s->isYAxisInverted());
        game->getInputManager()->setInvertY(s->isYAxisInverted());
        setupSettingsMenu();  // Refresh menu
    };
    currentMenuItems.push_back(invertYItem);
    
    MenuItem hudItem;
    hudItem.text = settings->isHUDEnabled() ? "HUD: ON" : "HUD: OFF";
    hudItem.onSelect = [this]() {
        auto s = game->getSettingsManager();
        s->setHUDEnabled(!s->isHUDEnabled());
        setupSettingsMenu();
    };
    currentMenuItems.push_back(hudItem);
    
    MenuItem backItem;
    backItem.text = "BACK";
    backItem.onSelect = [this]() {
        if (game->getState() == GameState::PAUSED) {
            showPauseMenu();
        } else {
            showMainMenu();
        }
    };
    currentMenuItems.push_back(backItem);
}

void MenuSystem::setupControlsMenu() {
    currentMenuItems.clear();
    selectedIndex = 0;
    
    MenuItem pitchItem;
    pitchItem.text = "PITCH UP/DOWN: W/S or UP/DOWN ARROWS";
    currentMenuItems.push_back(pitchItem);
    
    MenuItem rollItem;
    rollItem.text = "ROLL LEFT/RIGHT: A/D or LEFT/RIGHT ARROWS";
    currentMenuItems.push_back(rollItem);
    
    MenuItem yawItem;
    yawItem.text = "YAW LEFT/RIGHT: Z/X KEYS";
    currentMenuItems.push_back(yawItem);
    
    MenuItem throttleItem;
    throttleItem.text = "THROTTLE: E (up) / Q (down)";
    currentMenuItems.push_back(throttleItem);
    
    MenuItem brakeItem;
    brakeItem.text = "BRAKE: B KEY (hold)";
    currentMenuItems.push_back(brakeItem);
    
    MenuItem gearItem;
    gearItem.text = "LANDING GEAR: G KEY";
    currentMenuItems.push_back(gearItem);
    
    MenuItem flapsItem;
    flapsItem.text = "FLAPS: F (down) / V (up)";
    currentMenuItems.push_back(flapsItem);
    
    MenuItem cameraItem;
    cameraItem.text = "CAMERA: C KEY";
    currentMenuItems.push_back(cameraItem);
    
    MenuItem pauseItem;
    pauseItem.text = "PAUSE: ESC or P KEY";
    currentMenuItems.push_back(pauseItem);
    
    MenuItem resetItem;
    resetItem.text = "RESET TO DEFAULT";
    resetItem.onSelect = [this]() {
        game->getInputManager()->resetToDefaultMappings();
    };
    currentMenuItems.push_back(resetItem);
    
    MenuItem backItem;
    backItem.text = "BACK";
    backItem.onSelect = [this]() {
        showSettingsMenu();
    };
    currentMenuItems.push_back(backItem);
}

void MenuSystem::setupVolumeMenu() {
    currentMenuItems.clear();
    selectedIndex = 0;
    
    auto settings = game->getSettingsManager();
    char buffer[64];
    
    MenuItem masterItem;
    snprintf(buffer, sizeof(buffer), "MASTER: %.0f%%", settings->getMasterVolume() * 100);
    masterItem.text = buffer;
    currentMenuItems.push_back(masterItem);
    
    MenuItem musicItem;
    snprintf(buffer, sizeof(buffer), "MUSIC: %.0f%%", settings->getMusicVolume() * 100);
    musicItem.text = buffer;
    currentMenuItems.push_back(musicItem);
    
    MenuItem sfxItem;
    snprintf(buffer, sizeof(buffer), "SFX: %.0f%%", settings->getSFXVolume() * 100);
    sfxItem.text = buffer;
    currentMenuItems.push_back(sfxItem);
    
    MenuItem engineItem;
    snprintf(buffer, sizeof(buffer), "ENGINE: %.0f%%", settings->getEngineVolume() * 100);
    engineItem.text = buffer;
    currentMenuItems.push_back(engineItem);
    
    MenuItem backItem;
    backItem.text = "BACK";
    backItem.onSelect = [this]() {
        showSettingsMenu();
    };
    currentMenuItems.push_back(backItem);
}

void MenuSystem::showMainMenu() {
    currentMenu = MenuState::MAIN;
    transitionAlpha = 0.0f;
    setupMainMenu();
}

void MenuSystem::showAircraftSelect() {
    currentMenu = MenuState::AIRCRAFT_SELECT;
    transitionAlpha = 0.0f;
    setupAircraftSelectMenu();
}

void MenuSystem::showPauseMenu() {
    currentMenu = MenuState::PAUSE;
    transitionAlpha = 0.0f;
    setupPauseMenu();
}

void MenuSystem::showSettingsMenu() {
    currentMenu = MenuState::SETTINGS;
    transitionAlpha = 0.0f;
    setupSettingsMenu();
}

void MenuSystem::showControlsSettings() {
    currentMenu = MenuState::CONTROLS;
    transitionAlpha = 0.0f;
    setupControlsMenu();
}

void MenuSystem::showVolumeSettings() {
    currentMenu = MenuState::VOLUME;
    transitionAlpha = 0.0f;
    setupVolumeMenu();
}

void MenuSystem::renderMainMenu(Renderer* renderer) {
    int w = renderer->getWidth();
    int h = renderer->getHeight();
    
    // Background gradient
    renderer->renderRect(0, 0, w, h, Color(0.05f, 0.08f, 0.15f, 1.0f), true);
    
    // Title
    float titleY = h * 0.2f;
    Color titleColor(0.2f, 0.6f, 1.0f, transitionAlpha);
    renderer->renderText("FLIGHT SIMULATOR PRO", w/2 - 180, titleY, 2.0f, titleColor);
    
    // Subtitle
    renderer->renderText("The Ultimate Flying Experience", w/2 - 150, titleY + 50, 1.0f, 
                        Color(0.6f, 0.6f, 0.7f, transitionAlpha));
    
    // Menu items
    float menuY = h * 0.45f;
    float itemSpacing = 50.0f;
    float itemHeight = 40.0f;
    
    // Update item rects for mouse detection
    menuItemRects.clear();
    menuItemRects.resize(currentMenuItems.size());
    
    for (int i = 0; i < (int)currentMenuItems.size(); i++) {
        bool isSelected = (i == selectedIndex);
        
        // Store item rect for mouse detection
        menuItemRects[i] = {(float)(w/2 - 150), menuY + i * itemSpacing - 5, 300.0f, itemHeight};
        
        Color itemColor;
        if (!currentMenuItems[i].enabled) {
            itemColor = Color(0.4f, 0.4f, 0.4f, transitionAlpha);
        } else if (isSelected) {
            float pulse = 0.5f + 0.5f * std::sin(animationTime * 4.0f);
            itemColor = Color(0.3f + pulse * 0.2f, 0.8f + pulse * 0.2f, 1.0f, transitionAlpha);
        } else {
            itemColor = Color(0.7f, 0.7f, 0.8f, transitionAlpha);
        }
        
        // Selection indicator
        if (isSelected) {
            float indicatorX = w/2 - 120;
            renderer->renderText(">", indicatorX, menuY + i * itemSpacing, 1.5f, itemColor);
        }
        
        renderer->renderText(currentMenuItems[i].text.c_str(), 
                           w/2 - 80, menuY + i * itemSpacing, 1.5f, itemColor);
    }
    
    // Controller status
    auto input = game->getInputManager();
    std::string controllerStatus = input->isControllerConnected() 
        ? "Controller: " + input->getControllerName()
        : "No controller - Using keyboard";
    renderer->renderText(controllerStatus.c_str(), 20, h - 30, 0.8f, 
                        Color(0.5f, 0.5f, 0.5f, transitionAlpha));
    
    // Version
    renderer->renderText("v1.0.0", w - 80, h - 30, 0.8f, 
                        Color(0.5f, 0.5f, 0.5f, transitionAlpha));
}

void MenuSystem::renderAircraftSelect(Renderer* renderer) {
    int w = renderer->getWidth();
    int h = renderer->getHeight();
    
    // Background
    renderer->renderRect(0, 0, w, h, Color(0.05f, 0.08f, 0.15f, 1.0f), true);
    
    // Title
    renderer->renderText("SELECT YOUR AIRCRAFT", w/2 - 150, 30, 1.8f, 
                        Color(0.2f, 0.6f, 1.0f, transitionAlpha));
    
    // Get current aircraft info
    AircraftType currentType = availableAircraft[selectedAircraftIndex];
    Aircraft tempAircraft(currentType);
    const AircraftSpecs& specs = tempAircraft.getSpecs();
    
    // Clear and setup menu rects for aircraft selection
    menuItemRects.clear();
    
    // Display 3 aircraft options
    float aircraftBoxWidth = 200.0f;
    float aircraftBoxHeight = 280.0f;
    float spacing = 40.0f;
    float startX = (w - (3 * aircraftBoxWidth + 2 * spacing)) / 2.0f;
    float aircraftBoxY = 120.0f;
    
    for (int i = 0; i < 3 && i < (int)availableAircraft.size(); i++) {
        int aircraftIdx = (selectedAircraftIndex - 1 + i) % availableAircraft.size();
        AircraftType type = availableAircraft[aircraftIdx];
        Aircraft aircraft(type);
        const AircraftSpecs& spec = aircraft.getSpecs();
        
        float boxX = startX + i * (aircraftBoxWidth + spacing);
        bool isSelected = (i == 1); // Middle one is selected
        
        // Box background
        Color boxColor = isSelected 
            ? Color(0.3f, 0.6f, 1.0f, 0.9f)
            : Color(0.15f, 0.25f, 0.4f, 0.6f);
        Color borderColor = isSelected 
            ? Color(0.5f, 0.8f, 1.0f, 1.0f)
            : Color(0.3f, 0.5f, 0.8f, 0.6f);
        
        renderer->renderRect(boxX, aircraftBoxY, aircraftBoxWidth, aircraftBoxHeight, boxColor, true);
        renderer->renderRect(boxX, aircraftBoxY, aircraftBoxWidth, aircraftBoxHeight, borderColor, false);
        
        // Store clickable rect for this aircraft
        menuItemRects.push_back({boxX, aircraftBoxY, aircraftBoxWidth, aircraftBoxHeight});
        
        // Aircraft preview (simplified)
        float previewCenterX = boxX + aircraftBoxWidth / 2.0f;
        float previewCenterY = aircraftBoxY + 80.0f;
        float scale = 0.8f;
        
        // Draw simple aircraft shape
        Color primaryColor = isSelected ? Color(0.3f, 1.0f, 0.5f) : Color(0.6f, 0.8f, 1.0f);
        Color secondaryColor = isSelected ? Color(0.5f, 0.8f, 0.3f) : Color(0.5f, 0.7f, 0.9f);
        
        // Fuselage
        renderer->renderRect(previewCenterX - 8 * scale, previewCenterY - 40 * scale,
                            16 * scale, 80 * scale, primaryColor, true);
        // Wings
        renderer->renderRect(previewCenterX - 50 * scale, previewCenterY - 10 * scale,
                            100 * scale, 20 * scale, secondaryColor, true);
        // Tail
        renderer->renderRect(previewCenterX - 20 * scale, previewCenterY + 35 * scale,
                            40 * scale, 15 * scale, secondaryColor, true);
        
        // Aircraft name
        renderer->renderText(spec.name.c_str(), boxX + 10, aircraftBoxY + 190, 0.95f,
                            isSelected ? Color::White() : Color(0.8f, 0.8f, 0.9f));
        
        // Max speed
        char speedBuf[32];
        snprintf(speedBuf, sizeof(speedBuf), "%.0f km/h", spec.maxSpeed);
        renderer->renderText(speedBuf, boxX + 10, aircraftBoxY + 220, 0.8f,
                            Color(0.7f, 0.8f, 0.9f));
        
        // Stall speed
        char stallBuf[32];
        snprintf(stallBuf, sizeof(stallBuf), "Stall: %.0f", spec.stallSpeed);
        renderer->renderText(stallBuf, boxX + 10, aircraftBoxY + 245, 0.8f,
                            Color(0.7f, 0.8f, 0.9f));
    }
    
    // Navigation arrows
    float arrowY = aircraftBoxY + aircraftBoxHeight / 2.0f;
    renderer->renderText("<", 30, arrowY, 2.0f, Color(0.4f, 0.7f, 1.0f));
    renderer->renderText(">", w - 60, arrowY, 2.0f, Color(0.4f, 0.7f, 1.0f));
    
    // Selected aircraft details panel
    float detailPanelY = aircraftBoxY + aircraftBoxHeight + 40.0f;
    renderer->renderRect(100, detailPanelY, w - 200, 150, Color(0.1f, 0.15f, 0.25f, 0.8f), true);
    renderer->renderRect(100, detailPanelY, w - 200, 150, Color(0.3f, 0.6f, 1.0f, 0.7f), false);
    
    renderer->renderText(specs.name.c_str(), 120, detailPanelY + 15, 1.3f, Color::White());
    renderer->renderText(specs.description.c_str(), 120, detailPanelY + 45, 0.9f, 
                        Color(0.7f, 0.8f, 0.9f));
    
    char detailBuf[256];
    snprintf(detailBuf, sizeof(detailBuf), "Max Altitude: %.0f m | Climb: %.1f m/s | Weight: %.0f kg | Wingspan: %.1f m",
            specs.maxAltitude, specs.climbRate, specs.weight, specs.wingSpan);
    renderer->renderText(detailBuf, 120, detailPanelY + 75, 0.85f, 
                        Color(0.6f, 0.7f, 0.8f));
    
    // FLY button
    float flyButtonY = detailPanelY + 160.0f;
    float flyButtonX = w/2 - 100;
    float flyButtonW = 200.0f;
    float flyButtonH = 50.0f;
    
    bool flySelected = (selectedIndex == 0);
    Color flyColor = flySelected ? Color(0.3f, 1.0f, 0.5f) : Color(0.6f, 0.7f, 0.8f);
    
    renderer->renderRect(flyButtonX, flyButtonY, flyButtonW, flyButtonH,
                        flySelected ? Color(0.2f, 0.4f, 0.6f, 0.5f) : Color(0.15f, 0.25f, 0.4f, 0.3f), true);
    renderer->renderRect(flyButtonX, flyButtonY, flyButtonW, flyButtonH, Color(0.4f, 0.7f, 1.0f), false);
    renderer->renderText("FLY THIS AIRCRAFT", flyButtonX + 30, flyButtonY + 12, 1.2f, flyColor);
    
    // Store FLY button rect
    menuItemRects.push_back({flyButtonX, flyButtonY, flyButtonW, flyButtonH});
    
    // Hints
    renderer->renderText("Click aircraft or use LEFT/RIGHT arrows to select", w/2 - 220, h - 60, 0.85f,
                        Color(0.5f, 0.6f, 0.7f));
    renderer->renderText("Free Roam Mode - Fly anywhere you want!", w/2 - 200, h - 30, 0.9f,
                        Color(0.3f, 0.8f, 0.5f));
}

void MenuSystem::renderAircraftPreview(Renderer* renderer, AircraftType type) {
    // Simple 2D representation of aircraft for preview
    int w = renderer->getWidth();
    int h = renderer->getHeight();
    float centerX = w / 2.0f;
    float centerY = h * 0.38f;
    
    Aircraft tempAircraft(type);
    const AircraftSpecs& specs = tempAircraft.getSpecs();
    
    // Scale based on aircraft size
    float scale = 3.0f;
    
    // Draw aircraft silhouette
    Color primaryColor = specs.primaryColor;
    Color secondaryColor = specs.secondaryColor;
    
    // Fuselage
    float fuselageLength = specs.length * scale;
    float fuselageWidth = specs.length * 0.15f * scale;
    renderer->renderRect(centerX - fuselageWidth/2, centerY - fuselageLength/2, 
                        fuselageWidth, fuselageLength, primaryColor, true);
    
    // Wings
    float wingspan = specs.wingSpan * scale;
    float wingChord = specs.length * 0.25f * scale;
    renderer->renderRect(centerX - wingspan/2, centerY - wingChord/3, 
                        wingspan, wingChord, secondaryColor, true);
    
    // Tail
    float tailWidth = wingspan * 0.35f;
    float tailChord = wingChord * 0.6f;
    renderer->renderRect(centerX - tailWidth/2, centerY + fuselageLength/2 - tailChord, 
                        tailWidth, tailChord, secondaryColor, true);
    
    // Vertical stabilizer
    float stabHeight = fuselageLength * 0.2f;
    float stabWidth = fuselageWidth * 0.5f;
    renderer->renderRect(centerX - stabWidth/2, centerY + fuselageLength/3, 
                        stabWidth, stabHeight, primaryColor, true);
    
    // Cockpit/nose
    renderer->renderCircle(centerX, centerY - fuselageLength/2 + 10, 
                          fuselageWidth * 0.4f, Color(0.2f, 0.3f, 0.4f), true);
}

void MenuSystem::renderPauseMenu(Renderer* renderer) {
    int w = renderer->getWidth();
    int h = renderer->getHeight();
    
    // Semi-transparent overlay
    renderer->renderRect(0, 0, w, h, Color(0.0f, 0.0f, 0.0f, 0.7f), true);
    
    // Pause panel (top right as requested)
    float panelWidth = 300;
    float panelHeight = 350;
    float panelX = w - panelWidth - 30;
    float panelY = 30;
    
    // Panel background
    renderer->renderRect(panelX, panelY, panelWidth, panelHeight, 
                        Color(0.1f, 0.12f, 0.18f, 0.95f), true);
    renderer->renderRect(panelX, panelY, panelWidth, panelHeight, 
                        Color(0.3f, 0.5f, 0.8f, 0.8f), false);
    
    // Title
    renderer->renderText("PAUSED", panelX + 100, panelY + 20, 1.5f, 
                        Color(0.3f, 0.7f, 1.0f));
    
    // Menu items
    float itemY = panelY + 70;
    float itemSpacing = 45.0f;
    float itemHeight = 30.0f;
    
    // Update item rects for mouse detection
    menuItemRects.clear();
    menuItemRects.resize(currentMenuItems.size());
    
    for (int i = 0; i < (int)currentMenuItems.size(); i++) {
        bool isSelected = (i == selectedIndex);
        
        // Store item rect for mouse detection
        menuItemRects[i] = {panelX + 10, itemY + i * itemSpacing - 5, panelWidth - 20, itemHeight};
        
        Color itemColor = isSelected 
            ? Color(0.3f, 1.0f, 0.5f) 
            : Color(0.7f, 0.7f, 0.8f);
        
        if (isSelected) {
            // Highlight background
            renderer->renderRect(panelX + 10, itemY + i * itemSpacing - 5, 
                                panelWidth - 20, 30, Color(0.2f, 0.3f, 0.5f, 0.5f), true);
            renderer->renderText(">", panelX + 20, itemY + i * itemSpacing, 1.0f, itemColor);
        }
        
        renderer->renderText(currentMenuItems[i].text.c_str(), 
                           panelX + 45, itemY + i * itemSpacing, 1.0f, itemColor);
    }
    
    // Hint
    renderer->renderText("ESC to resume | Click to select", panelX + 40, panelY + panelHeight - 30, 0.7f,
                        Color(0.5f, 0.5f, 0.5f));
}

void MenuSystem::renderSettingsMenu(Renderer* renderer) {
    int w = renderer->getWidth();
    int h = renderer->getHeight();
    
    // Background
    renderer->renderRect(0, 0, w, h, Color(0.05f, 0.08f, 0.15f, 1.0f), true);
    
    // Title
    renderer->renderText("SETTINGS", w/2 - 80, 50, 1.8f, 
                        Color(0.2f, 0.6f, 1.0f, transitionAlpha));
    
    // Menu items
    float menuY = h * 0.3f;
    float itemSpacing = 50.0f;
    float itemHeight = 40.0f;
    
    // Update item rects for mouse detection
    menuItemRects.clear();
    menuItemRects.resize(currentMenuItems.size());
    
    for (int i = 0; i < (int)currentMenuItems.size(); i++) {
        bool isSelected = (i == selectedIndex);
        
        // Store item rect for mouse detection
        menuItemRects[i] = {(float)w/2 - 150, menuY + i * itemSpacing - 5, 300, itemHeight};
        
        Color itemColor = isSelected 
            ? Color(0.3f, 1.0f, 0.5f, transitionAlpha) 
            : Color(0.7f, 0.7f, 0.8f, transitionAlpha);
        
        if (isSelected) {
            renderer->renderText(">", w/2 - 140, menuY + i * itemSpacing, 1.3f, itemColor);
        }
        
        renderer->renderText(currentMenuItems[i].text.c_str(), 
                           w/2 - 100, menuY + i * itemSpacing, 1.3f, itemColor);
    }
}

void MenuSystem::renderControlsSettings(Renderer* renderer) {
    int w = renderer->getWidth();
    int h = renderer->getHeight();
    
    // Background
    renderer->renderRect(0, 0, w, h, Color(0.05f, 0.08f, 0.15f, 1.0f), true);
    
    // Title
    renderer->renderText("CONTROLS", w/2 - 80, 50, 1.8f, 
                        Color(0.2f, 0.6f, 1.0f, transitionAlpha));
    
    // Controller section
    auto input = game->getInputManager();
    if (input->isControllerConnected()) {
        renderer->renderText("Controller: ", 100, 100, 1.0f, Color(0.5f, 0.5f, 0.6f));
        renderer->renderText(input->getControllerName().c_str(), 220, 100, 1.0f, Color(0.3f, 0.8f, 0.3f));
        
        renderer->renderText("Left Stick: Pitch/Roll", 100, 130, 0.9f, Color(0.7f, 0.7f, 0.8f));
        renderer->renderText("Right Stick: Yaw", 100, 155, 0.9f, Color(0.7f, 0.7f, 0.8f));
        renderer->renderText("Triggers: Throttle", 100, 180, 0.9f, Color(0.7f, 0.7f, 0.8f));
    } else {
        renderer->renderText("No controller connected", 100, 100, 1.0f, Color(0.8f, 0.4f, 0.3f));
    }
    
    // Keyboard controls
    renderer->renderText("KEYBOARD CONTROLS:", 100, 230, 1.1f, Color(0.5f, 0.7f, 1.0f));
    
    float menuY = 270;
    float itemSpacing = 28.0f;
    float itemHeight = 24.0f;
    
    // Update item rects for mouse detection
    menuItemRects.clear();
    menuItemRects.resize(currentMenuItems.size());
    
    for (int i = 0; i < (int)currentMenuItems.size(); i++) {
        bool isSelected = (i == selectedIndex);
        
        // Store item rect for mouse detection
        menuItemRects[i] = {95, menuY + i * itemSpacing - 3, (float)w - 190, itemHeight};
        
        Color itemColor = isSelected 
            ? Color(0.3f, 1.0f, 0.5f, transitionAlpha) 
            : Color(0.7f, 0.7f, 0.8f, transitionAlpha);
        
        if (isSelected) {
            renderer->renderRect(95, menuY + i * itemSpacing - 3, w - 190, 24, 
                                Color(0.2f, 0.3f, 0.4f, 0.4f), true);
        }
        
        renderer->renderText(currentMenuItems[i].text.c_str(), 
                           100, menuY + i * itemSpacing, 1.0f, itemColor);
    }
    
    // Hint
    renderer->renderText("Click item to select | Arrow keys to navigate", w/2 - 180, h - 50, 0.9f,
                        Color(0.4f, 0.4f, 0.5f));
}

void MenuSystem::renderVolumeSettings(Renderer* renderer) {
    int w = renderer->getWidth();
    int h = renderer->getHeight();
    
    // Background
    renderer->renderRect(0, 0, w, h, Color(0.05f, 0.08f, 0.15f, 1.0f), true);
    
    // Title
    renderer->renderText("VOLUME", w/2 - 60, 50, 1.8f, 
                        Color(0.2f, 0.6f, 1.0f, transitionAlpha));
    
    auto settings = game->getSettingsManager();
    
    float menuY = h * 0.3f;
    float itemSpacing = 60.0f;
    float barWidth = 300.0f;
    float barHeight = 20.0f;
    float labelX = w/2 - 200;
    float barX = w/2 - 50;
    
    // Volume sliders
    std::vector<std::pair<std::string, float>> volumes = {
        {"MASTER", settings->getMasterVolume()},
        {"MUSIC", settings->getMusicVolume()},
        {"SFX", settings->getSFXVolume()},
        {"ENGINE", settings->getEngineVolume()}
    };
    
    for (int i = 0; i < (int)volumes.size(); i++) {
        bool isSelected = (i == selectedIndex);
        
        Color labelColor = isSelected 
            ? Color(0.3f, 1.0f, 0.5f, transitionAlpha) 
            : Color(0.7f, 0.7f, 0.8f, transitionAlpha);
        
        // Label
        renderer->renderText(volumes[i].first.c_str(), labelX, menuY + i * itemSpacing, 1.2f, labelColor);
        
        // Progress bar
        Color bgColor(0.2f, 0.2f, 0.25f, transitionAlpha);
        Color fillColor = isSelected 
            ? Color(0.3f, 0.8f, 0.4f, transitionAlpha)
            : Color(0.3f, 0.5f, 0.7f, transitionAlpha);
        
        renderer->renderProgressBar(barX, menuY + i * itemSpacing - 2, barWidth, barHeight,
                                   volumes[i].second, bgColor, fillColor);
        
        // Percentage
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%.0f%%", volumes[i].second * 100);
        renderer->renderText(buffer, barX + barWidth + 20, menuY + i * itemSpacing, 1.0f, labelColor);
    }
    
    // Back button
    int backIndex = 4;
    bool isBackSelected = (selectedIndex == backIndex);
    Color backColor = isBackSelected 
        ? Color(0.3f, 1.0f, 0.5f, transitionAlpha) 
        : Color(0.7f, 0.7f, 0.8f, transitionAlpha);
    
    if (isBackSelected) {
        renderer->renderText(">", w/2 - 60, menuY + backIndex * itemSpacing, 1.2f, backColor);
    }
    renderer->renderText("BACK", w/2 - 30, menuY + backIndex * itemSpacing, 1.2f, backColor);
    
    // Hint
    renderer->renderText("LEFT/RIGHT to adjust volume", w/2 - 130, h - 50, 0.9f,
                        Color(0.4f, 0.4f, 0.5f));
}

void MenuSystem::updateVolumeSettings(float delta) {
    auto settings = game->getSettingsManager();
    switch (selectedIndex) {
        case 0: settings->setMasterVolume(settings->getMasterVolume() + delta); break;
        case 1: settings->setMusicVolume(settings->getMusicVolume() + delta); break;
        case 2: settings->setSFXVolume(settings->getSFXVolume() + delta); break;
        case 3: settings->setEngineVolume(settings->getEngineVolume() + delta); break;
    }
    // Apply to audio manager
    auto audio = game->getAudioManager();
    audio->setMasterVolume(settings->getMasterVolume());
    audio->setMusicVolume(settings->getMusicVolume());
    audio->setSFXVolume(settings->getSFXVolume());
    audio->setEngineVolume(settings->getEngineVolume());
}
