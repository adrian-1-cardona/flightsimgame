#include "Camera.h"
#include "Aircraft.h"
#include <cmath>

Camera::Camera() {
    position = Vector3(0, 30, 60);
    target = Vector3(0, 15, 0);
    up = Vector3(0, 1, 0);
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
    }
}

void Camera::updateCockpitCamera(float deltaTime, const Aircraft* aircraft) {
    Vector3 aircraftPos = aircraft->getPosition();
    Vector3 forward = aircraft->getForwardVector();
    Vector3 up_vec = aircraft->getUpVector();
    
    // Cockpit view: slightly above aircraft, looking forward
    position = aircraftPos + up_vec * 2.0f;
    target = position + forward * 100.0f;
    up = up_vec;
}

void Camera::updateChaseCamera(float deltaTime, const Aircraft* aircraft) {
    Vector3 aircraftPos = aircraft->getPosition();
    Vector3 forward = aircraft->getForwardVector();
    Vector3 right = aircraft->getRightVector();
    Vector3 up_vec = aircraft->getUpVector();
    
    // Chase camera: behind and above the aircraft
    Vector3 cameraOffset = forward * (-chaseDistance) + up_vec * chaseHeight;
    position = aircraftPos + cameraOffset;
    
    // Look at aircraft with slight forward bias
    target = aircraftPos + forward * 10.0f;
    up = Vector3(0, 1, 0);
}

void Camera::updateOrbitCamera(float deltaTime, const Aircraft* aircraft) {
    Vector3 aircraftPos = aircraft->getPosition();
    
    // Orbit camera: circle around aircraft
    float rad_yaw = orbitYaw * DEG_TO_RAD;
    float rad_pitch = orbitPitch * DEG_TO_RAD;
    
    float x = std::sin(rad_yaw) * std::cos(rad_pitch) * orbitDistance;
    float y = std::sin(rad_pitch) * orbitDistance;
    float z = std::cos(rad_yaw) * std::cos(rad_pitch) * orbitDistance;
    
    position = aircraftPos + Vector3(x, y, z);
    target = aircraftPos;
    up = Vector3(0, 1, 0);
}

void Camera::setMode(CameraMode mode) {
    currentMode = mode;
}

void Camera::cycleMode() {
    switch (currentMode) {
        case CameraMode::CHASE:
            setMode(CameraMode::COCKPIT);
            break;
        case CameraMode::COCKPIT:
            setMode(CameraMode::ORBIT);
            break;
        case CameraMode::ORBIT:
            setMode(CameraMode::CHASE);
            break;
    }
}

Vector3 Camera::getForward() const {
    Vector3 forward = target - position;
    return forward.normalized();
}

Vector3 Camera::getRight() const {
    Vector3 forward = getForward();
    Vector3 right = Vector3::cross(forward, up);
    return right.normalized();
}
