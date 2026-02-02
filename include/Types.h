#pragma once

#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

// Math constants
constexpr float PI = 3.14159265359f;
constexpr float DEG_TO_RAD = PI / 180.0f;
constexpr float RAD_TO_DEG = 180.0f / PI;

// Vector3 structure for 3D positions and directions
struct Vector3 {
    float x, y, z;
    
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }
    
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }
    
    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    
    Vector3& operator+=(const Vector3& other) {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }
    
    Vector3& operator-=(const Vector3& other) {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }
    
    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    
    Vector3 normalized() const {
        float len = length();
        if (len > 0) return *this * (1.0f / len);
        return *this;
    }
    
    static Vector3 cross(const Vector3& a, const Vector3& b) {
        return Vector3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }
    
    static float dot(const Vector3& a, const Vector3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
};

// Color structure
struct Color {
    float r, g, b, a;
    
    Color() : r(1), g(1), b(1), a(1) {}
    Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
    
    static Color White() { return Color(1, 1, 1, 1); }
    static Color Black() { return Color(0, 0, 0, 1); }
    static Color Red() { return Color(1, 0, 0, 1); }
    static Color Green() { return Color(0, 1, 0, 1); }
    static Color Blue() { return Color(0, 0, 1, 1); }
    static Color SkyBlue() { return Color(0.529f, 0.808f, 0.922f, 1); }
    static Color Gray() { return Color(0.5f, 0.5f, 0.5f, 1); }
};

// Aircraft types
enum class AircraftType {
    BOEING_737,
    F16_FIGHTER,
    F22_RAPTOR,
    CESSNA_172,
    A320_AIRBUS
};

// Game states
enum class GameState {
    LOADING,
    MAIN_MENU,
    AIRCRAFT_SELECT,
    PLAYING,
    PAUSED,
    SETTINGS,
    CONTROLS_SETTINGS,
    QUIT
};

// Input action types
enum class InputAction {
    THROTTLE_UP,
    THROTTLE_DOWN,
    PITCH_UP,
    PITCH_DOWN,
    ROLL_LEFT,
    ROLL_RIGHT,
    YAW_LEFT,
    YAW_RIGHT,
    BRAKE,
    LANDING_GEAR,
    FLAPS_UP,
    FLAPS_DOWN,
    PAUSE,
    MENU_UP,
    MENU_DOWN,
    MENU_LEFT,
    MENU_RIGHT,
    MENU_SELECT,
    MENU_BACK,
    CAMERA_TOGGLE
};

// Controller button mapping
struct ControllerMapping {
    std::map<InputAction, int> buttonMappings;
    std::map<InputAction, int> axisMappings;
    std::map<InputAction, int> keyMappings;
    float deadzone = 0.15f;
};

// Aircraft specs
struct AircraftSpecs {
    std::string name;
    std::string description;
    AircraftType type;
    float maxSpeed;           // km/h
    float stallSpeed;         // km/h
    float maxAltitude;        // meters
    float climbRate;          // m/s
    float turnRate;           // degrees/s
    float acceleration;       // m/s^2
    float weight;             // kg
    float wingSpan;           // meters
    float length;             // meters
    Color primaryColor;
    Color secondaryColor;
};

// Settings structure
struct GameSettings {
    float masterVolume = 0.8f;
    float musicVolume = 0.7f;
    float sfxVolume = 0.8f;
    float engineVolume = 0.9f;
    bool fullscreen = false;
    int resolutionWidth = 1920;
    int resolutionHeight = 1080;
    bool vsync = true;
    float mouseSensitivity = 1.0f;
    float controllerSensitivity = 1.0f;
    bool invertYAxis = false;
    bool showHUD = true;
    bool showMinimap = true;
    int graphicsQuality = 2; // 0=Low, 1=Medium, 2=High, 3=Ultra
};

// Menu item
struct MenuItem {
    std::string text;
    bool enabled = true;
    bool selected = false;
    std::function<void()> onSelect;
};

// Runway info
struct Runway {
    float startX;      // Start X coordinate
    float startZ;      // Start Z coordinate  
    float endX;        // End X coordinate
    float endZ;        // End Z coordinate
    float width;       // Runway width
    float height;      // Runway height (Y coordinate)
};
