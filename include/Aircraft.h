#pragma once

#include "Types.h"

class Aircraft {
public:
    Aircraft(AircraftType type);
    ~Aircraft();
    
    void update(float deltaTime);
    void reset();
    
    // Control inputs (values from -1 to 1)
    void setThrottle(float value);
    void setPitch(float value);
    void setRoll(float value);
    void setYaw(float value);
    void setBrake(bool engaged);
    void toggleLandingGear();
    void adjustFlaps(int delta);
    
    // Setters for demo/spawn
    void setPosition(const Vector3& pos) { position = pos; onGround = (pos.y < 10); }
    
    // Getters
    const Vector3& getPosition() const { return position; }
    const Vector3& getRotation() const { return rotation; }
    const Vector3& getVelocity() const { return velocity; }
    const AircraftSpecs& getSpecs() const { return specs; }
    
    float getThrottle() const { return throttle; }
    float getSpeed() const { return speed; }
    float getAltitude() const { return position.y; }
    float getHeading() const { return rotation.y; }
    float getPitch() const { return rotation.x; }
    float getRoll() const { return rotation.z; }
    float getVerticalSpeed() const { return verticalSpeed; }
    int getFlapsLevel() const { return flapsLevel; }
    bool isLandingGearDown() const { return landingGearDown; }
    float getGearAnimationState() const { return gearAnimationState; }  // 0=up, 1=down
    float getFlapsAnimationState() const { return flapsLevel / 3.0f; }  // 0-1
    bool isOnGround() const { return onGround; }
    bool isStalling() const { return stalling; }
    
    // Forward and up vectors for camera
    Vector3 getForwardVector() const;
    Vector3 getUpVector() const;
    Vector3 getRightVector() const;
    
private:
    void applyPhysics(float deltaTime);
    void checkGroundCollision();
    void updateSpecs();
    
    AircraftSpecs specs;
    AircraftType type;
    
    // Position and orientation
    Vector3 position;
    Vector3 rotation;      // pitch, yaw, roll in degrees
    Vector3 velocity;
    Vector3 angularVelocity;
    
    // Flight parameters
    float throttle = 0.0f;
    float speed = 0.0f;           // Current speed in m/s
    float verticalSpeed = 0.0f;   // Climb/descent rate
    float pitchInput = 0.0f;
    float rollInput = 0.0f;
    float yawInput = 0.0f;
    
    // Aircraft state
    int flapsLevel = 0;           // 0-3
    bool landingGearDown = true;
    float gearAnimationState = 1.0f;  // 0=fully up, 1=fully down (animated)
    bool brakeEngaged = false;
    bool onGround = true;
    bool stalling = false;
    
    // Physics constants
    float dragCoefficient = 0.02f;
    float liftCoefficient = 0.5f;
    float gravity = 9.81f;
};
