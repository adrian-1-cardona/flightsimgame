#include "Physics.h"
#include "Aircraft.h"
#include "Terrain.h"
#include <cmath>
#include <algorithm>

Physics::Physics() {}

Physics::~Physics() {}

void Physics::update(float deltaTime, Aircraft* aircraft, const Terrain* terrain) {
    if (!aircraft) return;
    
    // The aircraft handles most of its own physics
    // This class is for more complex interactions
    
    // Check ground collision
    if (terrain) {
        handleGroundContact(aircraft, terrain);
        handleCollision(aircraft, terrain);
    }
}

Vector3 Physics::calculateLift(const Aircraft* aircraft) const {
    const AircraftSpecs& specs = aircraft->getSpecs();
    float speed = aircraft->getSpeed();
    float altitude = aircraft->getAltitude();
    
    // Air density decreases with altitude exponentially
    float airDensity = getAirDensityAtAltitude(altitude);
    
    // Lift equation: L = 0.5 * Cl * rho * v^2 * A
    float pitch = aircraft->getPitch();
    float roll = aircraft->getRoll();
    
    // Angle of attack from pitch (simplified)
    float angleOfAttack = pitch * 0.017453f;  // Convert to radians
    
    // Lift coefficient increases with angle of attack up to critical angle (~15°)
    float liftCoefficient = 0.5f;
    float criticalAngle = 0.26f;  // ~15 degrees in radians
    
    if (std::abs(angleOfAttack) < criticalAngle) {
        liftCoefficient = 0.4f + (std::abs(angleOfAttack) / criticalAngle) * 0.9f;
    } else {
        // Stall effect - dramatic lift loss
        liftCoefficient = 0.6f * std::exp(-std::pow((std::abs(angleOfAttack) - criticalAngle) / 0.3f, 2.0f));
    }
    
    // Effective wing area (simplified)
    float wingArea = specs.wingSpan * specs.length * 0.12f;
    
    // Lift magnitude: quadratic with speed
    float liftMagnitude = 0.5f * liftCoefficient * airDensity * speed * speed * wingArea / 1000.0f;
    
    // Flaps increase lift coefficient
    if (aircraft->getFlapsLevel() > 0) {
        liftMagnitude *= (1.0f + 0.15f * aircraft->getFlapsLevel());
    }
    
    // Lift direction is perpendicular to velocity (simplified - uses up vector)
    Vector3 liftDirection = aircraft->getUpVector();
    
    // Bank angle reduces effective vertical lift
    float bankAngle = roll * 0.017453f;
    liftMagnitude *= std::abs(std::cos(bankAngle));
    
    return liftDirection * liftMagnitude;
}

Vector3 Physics::calculateDrag(const Aircraft* aircraft) const {
    const AircraftSpecs& specs = aircraft->getSpecs();
    float speed = aircraft->getSpeed();
    float altitude = aircraft->getAltitude();
    
    float airDensity = getAirDensityAtAltitude(altitude);
    
    // Drag = 0.5 * Cd * rho * v^2 * A
    float dragCoefficient = 0.018f;  // Base drag coefficient (more aerodynamic)
    
    // Parasitic drag from landing gear and flaps
    if (aircraft->isLandingGearDown()) {
        dragCoefficient += 0.025f;  // Significant gear drag
    }
    
    // Flap drag increases dramatically with flap setting
    int flapsLevel = aircraft->getFlapsLevel();
    dragCoefficient += flapsLevel * 0.015f;
    
    // Induced drag (increases as lift increases - angle of attack effect)
    float pitch = std::abs(aircraft->getPitch());
    float inducedDrag = 0.001f * (pitch / 10.0f) * (pitch / 10.0f);
    dragCoefficient += inducedDrag;
    
    // Frontal area (approximation)
    float frontalArea = specs.wingSpan * specs.length * 0.08f;
    
    // Calculate drag magnitude: quadratic with speed
    float dragMagnitude = 0.5f * dragCoefficient * airDensity * speed * speed * frontalArea / 100.0f;
    
    // Drag direction is opposite to velocity
    Vector3 velocity = aircraft->getVelocity();
    if (velocity.length() > 0.1f) {
        return velocity.normalized() * (-dragMagnitude);
    }
    
    return Vector3(0, 0, 0);
}

Vector3 Physics::calculateThrust(const Aircraft* aircraft) const {
    const AircraftSpecs& specs = aircraft->getSpecs();
    float throttle = aircraft->getThrottle();
    
    // Maximum thrust based on aircraft specs
    float maxThrust = specs.weight * specs.acceleration;
    float thrust = maxThrust * throttle;
    
    // Thrust direction is along aircraft forward vector
    return aircraft->getForwardVector() * thrust;
}

Vector3 Physics::calculateWeight(const Aircraft* aircraft) const {
    const AircraftSpecs& specs = aircraft->getSpecs();
    return Vector3(0, -specs.weight * GRAVITY, 0);
}

float Physics::getAirDensityAtAltitude(float altitude) const {
    // Approximate air density using barometric formula
    // rho = rho_0 * exp(-altitude / H)
    // H ≈ 8500m for Earth's atmosphere
    float scaleHeight = 8500.0f;
    return AIR_DENSITY * std::exp(-altitude / scaleHeight);
}

float Physics::calculateStallSpeed(const Aircraft* aircraft) const {
    const AircraftSpecs& specs = aircraft->getSpecs();
    
    // Stall speed reduced by flaps
    float stallSpeed = specs.stallSpeed;
    stallSpeed *= (1.0f - 0.05f * aircraft->getFlapsLevel());
    
    // Stall speed increases with altitude (thinner air)
    float altitude = aircraft->getAltitude();
    float densityRatio = getAirDensityAtAltitude(altitude) / AIR_DENSITY;
    stallSpeed /= std::sqrt(densityRatio);
    
    return stallSpeed;
}

bool Physics::isStalling(const Aircraft* aircraft) const {
    if (aircraft->isOnGround()) return false;
    
    float speedKmh = aircraft->getSpeed() * 3.6f;
    float stallSpeed = calculateStallSpeed(aircraft);
    
    return speedKmh < stallSpeed;
}

void Physics::handleGroundContact(Aircraft* aircraft, const Terrain* terrain) {
    Vector3 pos = aircraft->getPosition();
    
    // Get terrain height at aircraft position
    float terrainHeight = terrain->getHeightAt(pos.x, pos.z);
    
    // Check if on runway (flat surface)
    if (terrain->isOnRunway(pos)) {
        terrainHeight = 0.0f;  // Runway is at sea level
    }
    
    // Ground contact is handled by Aircraft class
    // This could add additional effects like:
    // - Dust particles on dirt
    // - Sparks on hard landing
    // - Different friction on runway vs grass
}

void Physics::handleCollision(Aircraft* aircraft, const Terrain* terrain) {
    Vector3 pos = aircraft->getPosition();
    
    // Get terrain height
    float terrainHeight = terrain->getHeightAt(pos.x, pos.z);
    if (terrain->isOnRunway(pos)) {
        terrainHeight = 0.0f;
    }
    
    // Check for terrain collision (not on runway/flat ground)
    if (!aircraft->isOnGround() && pos.y < terrainHeight + 5.0f) {
        // Potential crash - for now just prevent going below terrain
        // In full implementation, this would trigger crash sequence
        
        // Check if landing conditions are acceptable
        float verticalSpeed = aircraft->getVerticalSpeed();
        float roll = std::abs(aircraft->getRoll());
        float pitch = std::abs(aircraft->getPitch());
        
        if (verticalSpeed < -5.0f || roll > 15.0f || pitch > 20.0f) {
            // Hard landing or crash
            // Could trigger crash animation/reset
        }
    }
}

void Physics::applyForces(Aircraft* aircraft, float deltaTime) {
    // Calculate all forces
    Vector3 lift = calculateLift(aircraft);
    Vector3 drag = calculateDrag(aircraft);
    Vector3 thrust = calculateThrust(aircraft);
    Vector3 weight = calculateWeight(aircraft);
    
    // Net force
    Vector3 netForce = lift + drag + thrust + weight;
    
    // F = ma, so a = F/m
    const AircraftSpecs& specs = aircraft->getSpecs();
    Vector3 acceleration = netForce * (1.0f / specs.weight);
    
    // This would update aircraft velocity if we wanted full physics control
    // Currently, aircraft handles its own simplified physics
}
