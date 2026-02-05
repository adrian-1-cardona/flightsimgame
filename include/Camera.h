#pragma once

#include "Types.h"

class Aircraft;

enum class CameraMode {
    CHASE,          // Third person behind aircraft (default)
    COCKPIT,        // First person from cockpit
    ORBIT           // Orbit around aircraft
};

class Camera {
public:
    Camera();
    ~Camera();
    
    void update(float deltaTime, const Aircraft* target);
    
    // Camera mode
    void setMode(CameraMode mode);
    CameraMode getMode() const { return currentMode; }
    void cycleMode();
    
    // Getters
    const Vector3& getPosition() const { return position; }
    const Vector3& getTarget() const { return target; }
    const Vector3& getUp() const { return up; }
    float getFOV() const { return fov; }
    float getNearPlane() const { return nearPlane; }
    float getFarPlane() const { return farPlane; }
    
    // View matrix components
    Vector3 getForward() const;
    Vector3 getRight() const;
    
    // Settings
    void setFOV(float fieldOfView) { fov = fieldOfView; }
    
private:
    void updateChaseCamera(float deltaTime, const Aircraft* target);
    void updateCockpitCamera(float deltaTime, const Aircraft* target);
    void updateOrbitCamera(float deltaTime, const Aircraft* target);
    
    CameraMode currentMode = CameraMode::CHASE;
    
    Vector3 position;
    Vector3 target;
    Vector3 up;
    
    // Chase camera parameters
    float chaseDistance = 25.0f;
    float chaseHeight = 8.0f;
    
    // Orbit camera parameters
    float orbitDistance = 40.0f;
    float orbitPitch = 15.0f;
    float orbitYaw = 0.0f;
    
    // Camera properties
    float fov = 60.0f;
    float nearPlane = 0.1f;
    float farPlane = 10000.0f;};