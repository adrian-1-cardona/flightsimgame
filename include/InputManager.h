#pragma once

#include "Types.h"
#include <SDL2/SDL.h>
#include <map>
#include <vector>
#include <string>
#include <set>

class InputManager {
public:
    InputManager();
    ~InputManager();
    
    bool initialize();
    void shutdown();
    void update();
    
    // Process SDL events
    void processEvent(const SDL_Event& event);
    
    // Controller management
    void scanForControllers();
    bool isControllerConnected() const { return activeController != nullptr; }
    std::string getControllerName() const;
    int getControllerCount() const { return static_cast<int>(connectedControllers.size()); }
    
    // Input queries
    bool isActionPressed(InputAction action) const;
    bool isActionJustPressed(InputAction action) const;
    bool isActionJustReleased(InputAction action) const;
    float getActionValue(InputAction action) const;
    
    // Axis values for flight controls (-1 to 1)
    float getPitchAxis() const;
    float getRollAxis() const;
    float getYawAxis() const;
    float getThrottleAxis() const;
    
    // Key state
    bool isKeyDown(SDL_Scancode key) const;
    bool isKeyJustPressed(SDL_Scancode key) const;
    bool wasKeyPressedThisFrame(SDL_Scancode key) const;
    void clearKeyPressesThisFrame();
    
    // Mouse state
    int getMouseX() const { return mouseX; }
    int getMouseY() const { return mouseY; }
    int getMouseDeltaX() const { return mouseDeltaX; }
    int getMouseDeltaY() const { return mouseDeltaY; }
    float getScrollDelta() const { return scrollDelta; }
    bool isMouseButtonDown(int button) const;
    bool isMouseButtonJustPressed(int button) const;
    bool wasMouseButtonClickedThisFrame(int button) const { return (mouseClickedThisFrame & SDL_BUTTON(button)) != 0; }
    void clearMouseClicksThisFrame() { mouseClickedThisFrame = 0; }
    void clearScrollDelta() { scrollDelta = 0.0f; }
    
    // Control customization
    void setKeyMapping(InputAction action, SDL_Scancode key);
    void setControllerButtonMapping(InputAction action, int button);
    void setControllerAxisMapping(InputAction action, int axis);
    ControllerMapping& getControllerMapping() { return controllerMapping; }
    void resetToDefaultMappings();
    void setDeadzone(float deadzone) { controllerMapping.deadzone = deadzone; }
    
    // Settings
    void setInvertY(bool invert) { invertY = invert; }
    void setSensitivity(float sens) { sensitivity = sens; }
    
private:
    void setupDefaultMappings();
    float applyDeadzone(float value) const;
    
    // Controllers
    SDL_GameController* activeController = nullptr;
    std::vector<SDL_GameController*> connectedControllers;
    
    // Mappings
    ControllerMapping controllerMapping;
    
    // Current input state
    std::map<InputAction, bool> actionStates;
    std::map<InputAction, bool> previousActionStates;
    std::map<InputAction, float> axisValues;
    
    // Keyboard state
    const Uint8* keyboardState = nullptr;
    std::map<SDL_Scancode, bool> previousKeyStates;
    std::set<SDL_Scancode> keysPressedThisFrame;  // Tracks keys pressed THIS frame
    
    // Mouse state
    int mouseX = 0;
    int mouseY = 0;
    int mouseDeltaX = 0;
    int mouseDeltaY = 0;
    float scrollDelta = 0.0f;
    Uint32 mouseButtons = 0;
    Uint32 previousMouseButtons = 0;
    Uint32 mouseClickedThisFrame = 0;  // Tracks clicks that happened THIS frame
    
    // Settings
    bool invertY = false;
    float sensitivity = 1.0f;
};
