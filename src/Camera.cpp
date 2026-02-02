#include "Camera.h"
#include "Aircraft.h"
#include <cmath>
#include <algorithm>
#include <iostream>

Camera::Camera() {
    position = Vector3(0, 50, 100);
    target = Vector3(0, 0, 0);
    up = Vector3(0, 1, 0);
    positionVelocity = Vector3(0, 0, 0);
    targetVelocity = Vector3(0, 0, 0);
}

Camera::~Camera() {}

void Camera::update(float deltaTime, const Aircraft* aircraft) {
    if (!aircraft) return;
    
    switch (currentMode) {
        case CameraMode::COCKPIT:
            updateCockpitCamera(deltaTime, aircraft);
            break;
        case CameraMode::CHASE:
            updateChaseCamera(deltaTime, aircraft);
            break;
        case CameraMode::ORBIT:
            updateOrbitCamera(deltaTime, aircraft);
            break;
        case CameraMode::FLYBY:
            updateFlybyCamera(deltaTime, aircraft);
            break;
        case CameraMode::TOWER:
            updateTowerCamera(deltaTime, aircraft);
            break;
    }
    
    // Apply camera shake
    if (shakeIntensity > 0) {
        float shakeX = ((float)rand() / RAND_MAX - 0.5f) * shakeIntensity;
        float shakeY = ((float)rand() / RAND_MAX - 0.5f) * shakeIntensity;
        float shakeZ = ((float)rand() / RAND_MAX - 0.5f) * shakeIntensity;
        position += Vector3(shakeX, shakeY, shakeZ);
        
        shakeIntensity -= shakeDecay * deltaTime;
        if (shakeIntensity < 0) shakeIntensity = 0;
    }
}

void Camera::updateCockpitCamera(float deltaTime, const Aircraft* target_aircraft) {
    Vector3 aircraftPos = target_aircraft->getPosition();
    Vector3 forward = target_aircraft->getForwardVector();
    Vector3 up_vec = target_aircraft->getUpVector();
    
    // Position slightly above and behind the nose
    float cockpitHeight = 2.0f;
    position = aircraftPos + up_vec * cockpitHeight;
    
    // Look forward
    target = position + forward * 100.0f;
    up = up_vec;
}

void Camera::updateChaseCamera(float deltaTime, const Aircraft* target_aircraft) {
    Vector3 aircraftPos = target_aircraft->getPosition();
    Vector3 forward = target_aircraft->getForwardVector();
    Vector3 aircraftUp = target_aircraft->getUpVector();
    
    // Apply mouse rotation to chase camera angles
    Vector3 targetPos = aircraftPos - forward * chaseDistance + Vector3(0, chaseHeight, 0);
    
    // Apply orbit angles to chase camera position for mouse control
    float x = std::sin(orbitYaw * DEG_TO_RAD) * chaseDistance;
    float y = chaseHeight;
    float z = -std::cos(orbitYaw * DEG_TO_RAD) * chaseDistance;
    
    targetPos = aircraftPos + Vector3(x, y, z);
    
    // Smooth camera movement
    position = smoothDamp(position, targetPos, positionVelocity, 1.0f / smoothness, deltaTime);
    
    // Look at aircraft
    Vector3 targetLook = aircraftPos + forward * 10.0f;  // Look slightly ahead of aircraft
    target = smoothDamp(target, targetLook, targetVelocity, 1.0f / (smoothness * 2), deltaTime);
    
    // Keep up vector mostly vertical for stability
    up = Vector3(0, 1, 0);
}

void Camera::updateOrbitCamera(float deltaTime, const Aircraft* target_aircraft) {
    Vector3 aircraftPos = target_aircraft->getPosition();
    
    // Calculate orbit position
    float x = std::cos(orbitYaw * DEG_TO_RAD) * std::cos(orbitPitch * DEG_TO_RAD) * orbitDistance;
    float y = std::sin(orbitPitch * DEG_TO_RAD) * orbitDistance;
    float z = std::sin(orbitYaw * DEG_TO_RAD) * std::cos(orbitPitch * DEG_TO_RAD) * orbitDistance;
    
    Vector3 targetPos = aircraftPos + Vector3(x, y, z);
    
    // Smooth movement
    position = smoothDamp(position, targetPos, positionVelocity, 1.0f / smoothness, deltaTime);
    target = aircraftPos;
    up = Vector3(0, 1, 0);
}

void Camera::updateFlybyCamera(float deltaTime, const Aircraft* target_aircraft) {
    static Vector3 staticPosition = Vector3(0, 20, 0);
    static float lastUpdate = 0;
    
    Vector3 aircraftPos = target_aircraft->getPosition();
    
    // Place camera at fixed positions as aircraft flies by
    float distToCamera = (aircraftPos - staticPosition).length();
    
    if (distToCamera > 500.0f || distToCamera < 20.0f) {
        // Relocate camera ahead of aircraft
        Vector3 forward = target_aircraft->getForwardVector();
        staticPosition = aircraftPos + forward * 200.0f + Vector3(
            ((float)rand() / RAND_MAX - 0.5f) * 100.0f,
            20.0f + ((float)rand() / RAND_MAX) * 30.0f,
            ((float)rand() / RAND_MAX - 0.5f) * 100.0f
        );
    }
    
    position = staticPosition;
    target = aircraftPos;
    up = Vector3(0, 1, 0);
}

void Camera::updateTowerCamera(float deltaTime, const Aircraft* target_aircraft) {
    // Fixed tower position near runway
    static Vector3 towerPosition(100, 30, -200);
    
    position = towerPosition;
    target = target_aircraft->getPosition();
    up = Vector3(0, 1, 0);
}

void Camera::setMode(CameraMode mode) {
    currentMode = mode;
    
    // Reset mode-specific parameters
    if (mode == CameraMode::ORBIT) {
        orbitYaw = 0.0f;
        orbitPitch = 20.0f;
        orbitDistance = 50.0f;
    }
}

void Camera::cycleMode() {
    switch (currentMode) {
        case CameraMode::CHASE:
            setMode(CameraMode::COCKPIT);
            std::cout << "Camera: COCKPIT view" << std::endl;
            break;
        case CameraMode::COCKPIT:
            setMode(CameraMode::ORBIT);
            std::cout << "Camera: ORBIT view" << std::endl;
            break;
        case CameraMode::ORBIT:
            setMode(CameraMode::FLYBY);
            std::cout << "Camera: FLYBY view" << std::endl;
            break;
        case CameraMode::FLYBY:
            setMode(CameraMode::TOWER);
            std::cout << "Camera: TOWER view" << std::endl;
            break;
        case CameraMode::TOWER:
            setMode(CameraMode::CHASE);
            std::cout << "Camera: CHASE view" << std::endl;
            break;
    }
}

void Camera::rotate(float yaw, float pitch) {
    if (currentMode == CameraMode::ORBIT) {
        orbitYaw += yaw;
        orbitPitch += pitch;
        
        // Clamp pitch
        orbitPitch = std::max(-80.0f, std::min(80.0f, orbitPitch));
    } else if (currentMode == CameraMode::CHASE) {
        // In chase mode, rotate around the aircraft
        orbitYaw += yaw;
        orbitPitch += pitch * 0.5f;  // Less vertical sensitivity in chase
        
        // Clamp pitch for chase camera
        orbitPitch = std::max(-30.0f, std::min(30.0f, orbitPitch));
    }
}

void Camera::zoom(float delta) {
    if (currentMode == CameraMode::ORBIT) {
        orbitDistance -= delta * 5.0f;
        orbitDistance = std::max(10.0f, std::min(200.0f, orbitDistance));
    } else if (currentMode == CameraMode::CHASE) {
        chaseDistance -= delta * 2.0f;
        chaseDistance = std::max(10.0f, std::min(100.0f, chaseDistance));
        chaseHeight -= delta * 0.5f;  // Also adjust height slightly
        chaseHeight = std::max(3.0f, std::min(30.0f, chaseHeight));
    }
}

Vector3 Camera::getForward() const {
    return (target - position).normalized();
}

Vector3 Camera::getRight() const {
    return Vector3::cross(getForward(), up).normalized();
}

Vector3 Camera::smoothDamp(const Vector3& current, const Vector3& target_pos, 
                           Vector3& velocity, float smoothTime, float deltaTime) {
    // Approximate smooth damp
    float omega = 2.0f / smoothTime;
    float x = omega * deltaTime;
    float exp_factor = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
    
    Vector3 change = current - target_pos;
    Vector3 temp = (velocity + change * omega) * deltaTime;
    velocity = (velocity - temp * omega) * exp_factor;
    
    return target_pos + (change + temp) * exp_factor;
}
