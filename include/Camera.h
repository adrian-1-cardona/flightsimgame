#pragma once

#include "Types.h"

class Aircraft;

enum class CameraMode {
    COCKPIT,        // First person from cockpit
    CHASE,          // Third person behind aircraft
    ORBIT,          // Free orbit around aircraft
    FLYBY,          // Static camera with aircraft flying by
    TOWER           // View from control tower
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
    
    // Manual camera control (for orbit mode)
    void rotate(float yaw, float pitch);
    void zoom(float delta);
    
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
    void setSmoothness(float s) { smoothness = s; }
    
private:
    void updateCockpitCamera(float deltaTime, const Aircraft* target);
    void updateChaseCamera(float deltaTime, const Aircraft* target);
    void updateOrbitCamera(float deltaTime, const Aircraft* target);
    void updateFlybyCamera(float deltaTime, const Aircraft* target);
    void updateTowerCamera(float deltaTime, const Aircraft* target);
    
    Vector3 smoothDamp(const Vector3& current, const Vector3& target, 
                       Vector3& velocity, float smoothTime, float deltaTime);
    
    CameraMode currentMode = CameraMode::CHASE;
    
    Vector3 position;
    Vector3 target;
    Vector3 up;
    Vector3 positionVelocity;
    Vector3 targetVelocity;
    
    // Orbit camera parameters
    float orbitYaw = 0.0f;
    float orbitPitch = 20.0f;
    float orbitDistance = 50.0f;
    
    // Chase camera parameters
    float chaseDistance = 30.0f;
    float chaseHeight = 10.0f;
    
    // Camera properties
    float fov = 60.0f;
    float nearPlane = 0.1f;
    float farPlane = 10000.0f;
    float smoothness = 5.0f;
    
    // Shake effect
    float shakeIntensity = 0.0f;
    float shakeDecay = 5.0f;
};
