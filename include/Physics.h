#pragma once

#include "Types.h"

class Aircraft;
class Terrain;

class Physics {
public:
    Physics();
    ~Physics();
    
    void update(float deltaTime, Aircraft* aircraft, const Terrain* terrain);
    
    // Physics constants
    static constexpr float GRAVITY = 9.81f;
    static constexpr float AIR_DENSITY = 1.225f;  // kg/m³ at sea level
    
    // Calculate forces
    Vector3 calculateLift(const Aircraft* aircraft) const;
    Vector3 calculateDrag(const Aircraft* aircraft) const;
    Vector3 calculateThrust(const Aircraft* aircraft) const;
    Vector3 calculateWeight(const Aircraft* aircraft) const;
    
    // Utility
    float getAirDensityAtAltitude(float altitude) const;
    float calculateStallSpeed(const Aircraft* aircraft) const;
    bool isStalling(const Aircraft* aircraft) const;
    
private:
    void applyForces(Aircraft* aircraft, float deltaTime);
    void handleGroundContact(Aircraft* aircraft, const Terrain* terrain);
    void handleCollision(Aircraft* aircraft, const Terrain* terrain);
};
