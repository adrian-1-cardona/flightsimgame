#include "InputManager.h"
#include <iostream>
#include <algorithm>

InputManager::InputManager() {}

InputManager::~InputManager() {
    shutdown();
}

bool InputManager::initialize() {
    std::cout << "Initializing input manager..." << std::endl;
    
    // Initialize SDL game controller subsystem
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0) {
        std::cerr << "Failed to initialize game controller subsystem: " << SDL_GetError() << std::endl;
    }
    
    // Load game controller mappings
    SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
    
    // Setup default mappings
    setupDefaultMappings();
    
    // Get keyboard state
    keyboardState = SDL_GetKeyboardState(nullptr);
    
    return true;
}

void InputManager::shutdown() {
    // Close all connected controllers
    for (auto controller : connectedControllers) {
        if (controller) {
            SDL_GameControllerClose(controller);
        }
    }
    connectedControllers.clear();
    activeController = nullptr;
}

void InputManager::setupDefaultMappings() {
    // Keyboard mappings
    controllerMapping.keyMappings[InputAction::THROTTLE_UP] = SDL_SCANCODE_W;
    controllerMapping.keyMappings[InputAction::THROTTLE_DOWN] = SDL_SCANCODE_S;
    controllerMapping.keyMappings[InputAction::PITCH_UP] = SDL_SCANCODE_UP;
    controllerMapping.keyMappings[InputAction::PITCH_DOWN] = SDL_SCANCODE_DOWN;
    controllerMapping.keyMappings[InputAction::ROLL_LEFT] = SDL_SCANCODE_LEFT;
    controllerMapping.keyMappings[InputAction::ROLL_RIGHT] = SDL_SCANCODE_RIGHT;
    controllerMapping.keyMappings[InputAction::YAW_LEFT] = SDL_SCANCODE_A;
    controllerMapping.keyMappings[InputAction::YAW_RIGHT] = SDL_SCANCODE_D;
    controllerMapping.keyMappings[InputAction::BRAKE] = SDL_SCANCODE_B;
    controllerMapping.keyMappings[InputAction::LANDING_GEAR] = SDL_SCANCODE_G;
    controllerMapping.keyMappings[InputAction::FLAPS_UP] = SDL_SCANCODE_F;
    controllerMapping.keyMappings[InputAction::FLAPS_DOWN] = SDL_SCANCODE_V;
    controllerMapping.keyMappings[InputAction::PAUSE] = SDL_SCANCODE_ESCAPE;
    controllerMapping.keyMappings[InputAction::MENU_UP] = SDL_SCANCODE_UP;
    controllerMapping.keyMappings[InputAction::MENU_DOWN] = SDL_SCANCODE_DOWN;
    controllerMapping.keyMappings[InputAction::MENU_LEFT] = SDL_SCANCODE_LEFT;
    controllerMapping.keyMappings[InputAction::MENU_RIGHT] = SDL_SCANCODE_RIGHT;
    controllerMapping.keyMappings[InputAction::MENU_SELECT] = SDL_SCANCODE_RETURN;
    controllerMapping.keyMappings[InputAction::MENU_BACK] = SDL_SCANCODE_ESCAPE;
    controllerMapping.keyMappings[InputAction::CAMERA_TOGGLE] = SDL_SCANCODE_C;
    
    // Controller button mappings (Xbox-style)
    controllerMapping.buttonMappings[InputAction::BRAKE] = SDL_CONTROLLER_BUTTON_A;
    controllerMapping.buttonMappings[InputAction::LANDING_GEAR] = SDL_CONTROLLER_BUTTON_Y;
    controllerMapping.buttonMappings[InputAction::FLAPS_UP] = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
    controllerMapping.buttonMappings[InputAction::FLAPS_DOWN] = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
    controllerMapping.buttonMappings[InputAction::PAUSE] = SDL_CONTROLLER_BUTTON_START;
    controllerMapping.buttonMappings[InputAction::MENU_UP] = SDL_CONTROLLER_BUTTON_DPAD_UP;
    controllerMapping.buttonMappings[InputAction::MENU_DOWN] = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
    controllerMapping.buttonMappings[InputAction::MENU_LEFT] = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
    controllerMapping.buttonMappings[InputAction::MENU_RIGHT] = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
    controllerMapping.buttonMappings[InputAction::MENU_SELECT] = SDL_CONTROLLER_BUTTON_A;
    controllerMapping.buttonMappings[InputAction::MENU_BACK] = SDL_CONTROLLER_BUTTON_B;
    controllerMapping.buttonMappings[InputAction::CAMERA_TOGGLE] = SDL_CONTROLLER_BUTTON_X;
    
    // Controller axis mappings
    controllerMapping.axisMappings[InputAction::PITCH_UP] = SDL_CONTROLLER_AXIS_LEFTY;      // Left stick Y
    controllerMapping.axisMappings[InputAction::ROLL_LEFT] = SDL_CONTROLLER_AXIS_LEFTX;     // Left stick X
    controllerMapping.axisMappings[InputAction::YAW_LEFT] = SDL_CONTROLLER_AXIS_RIGHTX;     // Right stick X
    controllerMapping.axisMappings[InputAction::THROTTLE_UP] = SDL_CONTROLLER_AXIS_TRIGGERLEFT;   // Left trigger
    controllerMapping.axisMappings[InputAction::THROTTLE_DOWN] = SDL_CONTROLLER_AXIS_TRIGGERRIGHT; // Right trigger
}

void InputManager::resetToDefaultMappings() {
    controllerMapping.buttonMappings.clear();
    controllerMapping.axisMappings.clear();
    controllerMapping.keyMappings.clear();
    controllerMapping.deadzone = 0.15f;
    setupDefaultMappings();
}

void InputManager::scanForControllers() {
    std::cout << "Scanning for controllers..." << std::endl;
    
    // Close existing controllers
    for (auto controller : connectedControllers) {
        if (controller) {
            SDL_GameControllerClose(controller);
        }
    }
    connectedControllers.clear();
    activeController = nullptr;
    
    // Scan for new controllers
    int numJoysticks = SDL_NumJoysticks();
    std::cout << "Found " << numJoysticks << " joystick(s)" << std::endl;
    
    for (int i = 0; i < numJoysticks; i++) {
        if (SDL_IsGameController(i)) {
            SDL_GameController* controller = SDL_GameControllerOpen(i);
            if (controller) {
                connectedControllers.push_back(controller);
                std::cout << "  Controller " << i << ": " 
                         << SDL_GameControllerName(controller) << std::endl;
                
                // Use first controller as active
                if (!activeController) {
                    activeController = controller;
                }
            }
        }
    }
    
    if (activeController) {
        std::cout << "Active controller: " << SDL_GameControllerName(activeController) << std::endl;
    } else {
        std::cout << "No controller active. Using keyboard." << std::endl;
    }
}

std::string InputManager::getControllerName() const {
    if (activeController) {
        return SDL_GameControllerName(activeController);
    }
    return "No controller";
}

void InputManager::processEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            // Update button states
            for (const auto& mapping : controllerMapping.buttonMappings) {
                if (mapping.second == event.cbutton.button) {
                    actionStates[mapping.first] = (event.type == SDL_CONTROLLERBUTTONDOWN);
                }
            }
            break;
            
        case SDL_CONTROLLERAXISMOTION:
            // Update axis values
            for (const auto& mapping : controllerMapping.axisMappings) {
                if (mapping.second == event.caxis.axis) {
                    float value = event.caxis.value / 32767.0f;
                    axisValues[mapping.first] = applyDeadzone(value);
                }
            }
            break;
            
        case SDL_KEYDOWN:
            // Track that this key was pressed this frame
            keysPressedThisFrame.insert(event.key.keysym.scancode);
            for (const auto& mapping : controllerMapping.keyMappings) {
                if (mapping.second == event.key.keysym.scancode) {
                    actionStates[mapping.first] = true;
                }
            }
            break;
            
        case SDL_KEYUP:
            for (const auto& mapping : controllerMapping.keyMappings) {
                if (mapping.second == event.key.keysym.scancode) {
                    actionStates[mapping.first] = false;
                }
            }
            break;
            
        case SDL_MOUSEMOTION:
            mouseDeltaX = event.motion.xrel;
            mouseDeltaY = event.motion.yrel;
            mouseX = event.motion.x;
            mouseY = event.motion.y;
            break;
            
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                mouseButtons |= SDL_BUTTON(event.button.button);
                // Track that this button was clicked this frame
                mouseClickedThisFrame |= SDL_BUTTON(event.button.button);
            } else {
                mouseButtons &= ~SDL_BUTTON(event.button.button);
            }
            break;
            
        case SDL_MOUSEWHEEL:
            // Positive is scroll up/away (zoom out), negative is scroll down/towards (zoom in)
            scrollDelta = static_cast<float>(event.wheel.y);
            break;
    }
}

void InputManager::update() {
    // Store previous states
    previousActionStates = actionStates;
    
    // Clear mouse motion deltas at the start of update
    // (they're set in processEvent during the frame)
    // We'll reset them at the END of update
    
    // Save previous keyboard states for all important keys
    if (keyboardState) {
        // Menu navigation keys
        previousKeyStates[SDL_SCANCODE_UP] = keyboardState[SDL_SCANCODE_UP] != 0;
        previousKeyStates[SDL_SCANCODE_DOWN] = keyboardState[SDL_SCANCODE_DOWN] != 0;
        previousKeyStates[SDL_SCANCODE_LEFT] = keyboardState[SDL_SCANCODE_LEFT] != 0;
        previousKeyStates[SDL_SCANCODE_RIGHT] = keyboardState[SDL_SCANCODE_RIGHT] != 0;
        previousKeyStates[SDL_SCANCODE_RETURN] = keyboardState[SDL_SCANCODE_RETURN] != 0;
        previousKeyStates[SDL_SCANCODE_SPACE] = keyboardState[SDL_SCANCODE_SPACE] != 0;
        previousKeyStates[SDL_SCANCODE_ESCAPE] = keyboardState[SDL_SCANCODE_ESCAPE] != 0;
        previousKeyStates[SDL_SCANCODE_BACKSPACE] = keyboardState[SDL_SCANCODE_BACKSPACE] != 0;
        previousKeyStates[SDL_SCANCODE_W] = keyboardState[SDL_SCANCODE_W] != 0;
        previousKeyStates[SDL_SCANCODE_A] = keyboardState[SDL_SCANCODE_A] != 0;
        previousKeyStates[SDL_SCANCODE_S] = keyboardState[SDL_SCANCODE_S] != 0;
        previousKeyStates[SDL_SCANCODE_D] = keyboardState[SDL_SCANCODE_D] != 0;
        previousKeyStates[SDL_SCANCODE_G] = keyboardState[SDL_SCANCODE_G] != 0;
        previousKeyStates[SDL_SCANCODE_C] = keyboardState[SDL_SCANCODE_C] != 0;
        previousKeyStates[SDL_SCANCODE_P] = keyboardState[SDL_SCANCODE_P] != 0;
        previousKeyStates[SDL_SCANCODE_F] = keyboardState[SDL_SCANCODE_F] != 0;
        previousKeyStates[SDL_SCANCODE_V] = keyboardState[SDL_SCANCODE_V] != 0;
        previousKeyStates[SDL_SCANCODE_B] = keyboardState[SDL_SCANCODE_B] != 0;
        previousKeyStates[SDL_SCANCODE_Q] = keyboardState[SDL_SCANCODE_Q] != 0;
        previousKeyStates[SDL_SCANCODE_E] = keyboardState[SDL_SCANCODE_E] != 0;
        previousKeyStates[SDL_SCANCODE_Z] = keyboardState[SDL_SCANCODE_Z] != 0;
        previousKeyStates[SDL_SCANCODE_X] = keyboardState[SDL_SCANCODE_X] != 0;
    }
    
    // Track previous mouse button state for click detection
    previousMouseButtons = mouseButtons;
    
    // Update keyboard state
    keyboardState = SDL_GetKeyboardState(nullptr);
    
    // Update controller axis values if controller is connected
    if (activeController) {
        // Left stick for pitch/roll
        float leftX = SDL_GameControllerGetAxis(activeController, SDL_CONTROLLER_AXIS_LEFTX) / 32767.0f;
        float leftY = SDL_GameControllerGetAxis(activeController, SDL_CONTROLLER_AXIS_LEFTY) / 32767.0f;
        
        axisValues[InputAction::ROLL_LEFT] = applyDeadzone(leftX);
        axisValues[InputAction::PITCH_UP] = applyDeadzone(leftY);
        
        // Right stick for yaw
        float rightX = SDL_GameControllerGetAxis(activeController, SDL_CONTROLLER_AXIS_RIGHTX) / 32767.0f;
        axisValues[InputAction::YAW_LEFT] = applyDeadzone(rightX);
        
        // Triggers for throttle
        float leftTrigger = SDL_GameControllerGetAxis(activeController, SDL_CONTROLLER_AXIS_TRIGGERLEFT) / 32767.0f;
        float rightTrigger = SDL_GameControllerGetAxis(activeController, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 32767.0f;
        axisValues[InputAction::THROTTLE_UP] = leftTrigger;
        axisValues[InputAction::THROTTLE_DOWN] = rightTrigger;
    }
    
    // Reset mouse deltas at the end of update for next frame
    mouseDeltaX = 0;
    mouseDeltaY = 0;
}

float InputManager::applyDeadzone(float value) const {
    if (std::abs(value) < controllerMapping.deadzone) {
        return 0.0f;
    }
    // Remap value outside deadzone
    float sign = value > 0 ? 1.0f : -1.0f;
    float adjusted = (std::abs(value) - controllerMapping.deadzone) / (1.0f - controllerMapping.deadzone);
    return sign * adjusted * sensitivity;
}

bool InputManager::isActionPressed(InputAction action) const {
    auto it = actionStates.find(action);
    if (it != actionStates.end() && it->second) {
        return true;
    }
    
    // Check keyboard
    auto keyIt = controllerMapping.keyMappings.find(action);
    if (keyIt != controllerMapping.keyMappings.end() && keyboardState) {
        if (keyboardState[keyIt->second]) {
            return true;
        }
    }
    
    return false;
}

bool InputManager::isActionJustPressed(InputAction action) const {
    bool currentlyPressed = isActionPressed(action);
    auto prevIt = previousActionStates.find(action);
    bool previouslyPressed = (prevIt != previousActionStates.end()) && prevIt->second;
    
    // Also check keyboard previous state
    auto keyIt = controllerMapping.keyMappings.find(action);
    if (keyIt != controllerMapping.keyMappings.end()) {
        SDL_Scancode scancode = static_cast<SDL_Scancode>(keyIt->second);
        auto prevKeyIt = previousKeyStates.find(scancode);
        if (prevKeyIt != previousKeyStates.end()) {
            previouslyPressed = previouslyPressed || prevKeyIt->second;
        }
    }
    
    return currentlyPressed && !previouslyPressed;
}

bool InputManager::isActionJustReleased(InputAction action) const {
    bool currentlyPressed = isActionPressed(action);
    auto prevIt = previousActionStates.find(action);
    bool previouslyPressed = (prevIt != previousActionStates.end()) && prevIt->second;
    
    return !currentlyPressed && previouslyPressed;
}

float InputManager::getActionValue(InputAction action) const {
    auto it = axisValues.find(action);
    if (it != axisValues.end()) {
        return it->second;
    }
    return 0.0f;
}

float InputManager::getPitchAxis() const {
    // Controller input
    float controllerPitch = -getActionValue(InputAction::PITCH_UP);  // Invert for natural feel
    
    // Keyboard input - W/S and Up/Down for pitch
    float keyboardPitch = 0.0f;
    if (keyboardState) {
        if (keyboardState[SDL_SCANCODE_W] || keyboardState[SDL_SCANCODE_UP]) keyboardPitch -= 1.0f;
        if (keyboardState[SDL_SCANCODE_S] || keyboardState[SDL_SCANCODE_DOWN]) keyboardPitch += 1.0f;
    }
    
    float result = (std::abs(controllerPitch) > std::abs(keyboardPitch)) ? controllerPitch : keyboardPitch;
    
    if (invertY) result = -result;
    
    return result;
}

float InputManager::getRollAxis() const {
    // Controller input
    float controllerRoll = getActionValue(InputAction::ROLL_LEFT);
    
    // Keyboard input - A/D and Left/Right for roll
    float keyboardRoll = 0.0f;
    if (keyboardState) {
        if (keyboardState[SDL_SCANCODE_A] || keyboardState[SDL_SCANCODE_LEFT]) keyboardRoll -= 1.0f;
        if (keyboardState[SDL_SCANCODE_D] || keyboardState[SDL_SCANCODE_RIGHT]) keyboardRoll += 1.0f;
    }
    
    return (std::abs(controllerRoll) > std::abs(keyboardRoll)) ? controllerRoll : keyboardRoll;
}

float InputManager::getYawAxis() const {
    // Controller input
    float controllerYaw = getActionValue(InputAction::YAW_LEFT);
    
    // Keyboard input - Z/X for yaw
    float keyboardYaw = 0.0f;
    if (keyboardState) {
        if (keyboardState[SDL_SCANCODE_Z]) keyboardYaw -= 1.0f;
        if (keyboardState[SDL_SCANCODE_X]) keyboardYaw += 1.0f;
    }
    
    return (std::abs(controllerYaw) > std::abs(keyboardYaw)) ? controllerYaw : keyboardYaw;
}

float InputManager::getThrottleAxis() const {
    // Controller input (triggers)
    float throttleUp = getActionValue(InputAction::THROTTLE_UP);
    float throttleDown = getActionValue(InputAction::THROTTLE_DOWN);
    float controllerThrottle = throttleUp - throttleDown;
    
    // Keyboard input - E to increase, Q to decrease
    float keyboardThrottle = 0.0f;
    if (keyboardState) {
        if (keyboardState[SDL_SCANCODE_E]) keyboardThrottle += 1.0f;
        if (keyboardState[SDL_SCANCODE_Q]) keyboardThrottle -= 1.0f;
    }
    
    return (std::abs(controllerThrottle) > std::abs(keyboardThrottle)) ? controllerThrottle : keyboardThrottle;
}

bool InputManager::isKeyDown(SDL_Scancode key) const {
    if (keyboardState) {
        return keyboardState[key] != 0;
    }
    return false;
}

bool InputManager::isKeyJustPressed(SDL_Scancode key) const {
    if (!keyboardState) return false;
    
    bool current = keyboardState[key] != 0;
    auto it = previousKeyStates.find(key);
    bool previous = (it != previousKeyStates.end()) ? it->second : false;
    
    return current && !previous;
}

bool InputManager::wasKeyPressedThisFrame(SDL_Scancode key) const {
    return keysPressedThisFrame.find(key) != keysPressedThisFrame.end();
}

void InputManager::clearKeyPressesThisFrame() {
    keysPressedThisFrame.clear();
}

bool InputManager::isMouseButtonDown(int button) const {
    return (mouseButtons & SDL_BUTTON(button)) != 0;
}

bool InputManager::isMouseButtonJustPressed(int button) const {
    bool current = (mouseButtons & SDL_BUTTON(button)) != 0;
    bool previous = (previousMouseButtons & SDL_BUTTON(button)) != 0;
    return current && !previous;
}

void InputManager::setKeyMapping(InputAction action, SDL_Scancode key) {
    controllerMapping.keyMappings[action] = key;
}

void InputManager::setControllerButtonMapping(InputAction action, int button) {
    controllerMapping.buttonMappings[action] = button;
}

void InputManager::setControllerAxisMapping(InputAction action, int axis) {
    controllerMapping.axisMappings[action] = axis;
}
