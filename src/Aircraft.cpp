#include "Aircraft.h"
#include <cmath>
#include <algorithm>
#include <iostream>

Aircraft::Aircraft(AircraftType type) : type(type) {
    updateSpecs();
    reset();
}

Aircraft::~Aircraft() {}

void Aircraft::updateSpecs() {
    switch (type) {
        case AircraftType::BOEING_737:
            specs.name = "Boeing 737-800";
            specs.description = "Popular commercial airliner";
            specs.type = AircraftType::BOEING_737;
            specs.maxSpeed = 876.0f;      // km/h
            specs.stallSpeed = 220.0f;     // km/h
            specs.maxAltitude = 12500.0f;  // meters
            specs.climbRate = 15.0f;       // m/s
            specs.turnRate = 3.0f;         // degrees/s
            specs.acceleration = 3.0f;     // m/s^2
            specs.weight = 79000.0f;       // kg
            specs.wingSpan = 35.8f;        // meters
            specs.length = 39.5f;          // meters
            specs.primaryColor = Color(0.9f, 0.9f, 0.95f, 1.0f);  // White
            specs.secondaryColor = Color(0.0f, 0.3f, 0.6f, 1.0f); // Blue
            dragCoefficient = 0.025f;
            liftCoefficient = 0.5f;
            break;
            
        case AircraftType::F16_FIGHTER:
            specs.name = "F-16 Fighting Falcon";
            specs.description = "Multirole fighter jet";
            specs.type = AircraftType::F16_FIGHTER;
            specs.maxSpeed = 2120.0f;      // km/h (Mach 2)
            specs.stallSpeed = 280.0f;     // km/h
            specs.maxAltitude = 15240.0f;  // meters
            specs.climbRate = 254.0f;      // m/s
            specs.turnRate = 26.0f;        // degrees/s (9G turn)
            specs.acceleration = 15.0f;    // m/s^2
            specs.weight = 12000.0f;       // kg
            specs.wingSpan = 9.96f;        // meters
            specs.length = 15.06f;         // meters
            specs.primaryColor = Color(0.5f, 0.5f, 0.55f, 1.0f);  // Gray
            specs.secondaryColor = Color(0.3f, 0.35f, 0.4f, 1.0f); // Dark gray
            dragCoefficient = 0.015f;
            liftCoefficient = 0.7f;
            break;
            
        case AircraftType::F22_RAPTOR:
            specs.name = "F-22 Raptor";
            specs.description = "Stealth air superiority fighter";
            specs.type = AircraftType::F22_RAPTOR;
            specs.maxSpeed = 2410.0f;      // km/h (Mach 2.25)
            specs.stallSpeed = 260.0f;     // km/h
            specs.maxAltitude = 19812.0f;  // meters
            specs.climbRate = 280.0f;      // m/s
            specs.turnRate = 28.0f;        // degrees/s
            specs.acceleration = 18.0f;    // m/s^2
            specs.weight = 19700.0f;       // kg
            specs.wingSpan = 13.56f;       // meters
            specs.length = 18.92f;         // meters
            specs.primaryColor = Color(0.4f, 0.42f, 0.45f, 1.0f);  // Dark gray
            specs.secondaryColor = Color(0.25f, 0.27f, 0.3f, 1.0f); // Darker gray
            dragCoefficient = 0.012f;
            liftCoefficient = 0.75f;
            break;
            
        case AircraftType::CESSNA_172:
            specs.name = "Cessna 172 Skyhawk";
            specs.description = "Light single-engine aircraft";
            specs.type = AircraftType::CESSNA_172;
            specs.maxSpeed = 302.0f;       // km/h
            specs.stallSpeed = 87.0f;      // km/h
            specs.maxAltitude = 4100.0f;   // meters
            specs.climbRate = 3.66f;       // m/s
            specs.turnRate = 4.5f;         // degrees/s
            specs.acceleration = 2.0f;     // m/s^2
            specs.weight = 1111.0f;        // kg
            specs.wingSpan = 11.0f;        // meters
            specs.length = 8.28f;          // meters
            specs.primaryColor = Color(1.0f, 1.0f, 1.0f, 1.0f);   // White
            specs.secondaryColor = Color(0.8f, 0.0f, 0.0f, 1.0f); // Red
            dragCoefficient = 0.04f;
            liftCoefficient = 0.6f;
            break;
            
        case AircraftType::A320_AIRBUS:
            specs.name = "Airbus A320";
            specs.description = "Narrow-body commercial airliner";
            specs.type = AircraftType::A320_AIRBUS;
            specs.maxSpeed = 871.0f;       // km/h
            specs.stallSpeed = 213.0f;     // km/h
            specs.maxAltitude = 12000.0f;  // meters
            specs.climbRate = 13.0f;       // m/s
            specs.turnRate = 2.8f;         // degrees/s
            specs.acceleration = 2.8f;     // m/s^2
            specs.weight = 78000.0f;       // kg
            specs.wingSpan = 35.8f;        // meters
            specs.length = 37.57f;         // meters
            specs.primaryColor = Color(0.95f, 0.95f, 1.0f, 1.0f);  // White
            specs.secondaryColor = Color(0.0f, 0.2f, 0.5f, 1.0f);  // Navy blue
            dragCoefficient = 0.028f;
            liftCoefficient = 0.48f;
            break;
    }
}

void Aircraft::reset() {
    // Start on runway
    position = Vector3(0, 2.0f, -400.0f);
    rotation = Vector3(0, 0, 0);
    velocity = Vector3(0, 0, 0);
    angularVelocity = Vector3(0, 0, 0);
    
    throttle = 0.0f;
    speed = 0.0f;
    verticalSpeed = 0.0f;
    pitchInput = 0.0f;
    rollInput = 0.0f;
    yawInput = 0.0f;
    
    flapsLevel = 0;
    landingGearDown = true;
    brakeEngaged = false;
    onGround = true;
    stalling = false;
}

void Aircraft::update(float deltaTime) {
    applyPhysics(deltaTime);
    checkGroundCollision();
    
    // Animate landing gear (smooth transition)
    float targetGear = landingGearDown ? 1.0f : 0.0f;
    float gearSpeed = 0.5f;  // Takes 2 seconds to fully extend/retract
    if (gearAnimationState < targetGear) {
        gearAnimationState += gearSpeed * deltaTime;
        if (gearAnimationState > targetGear) gearAnimationState = targetGear;
    } else if (gearAnimationState > targetGear) {
        gearAnimationState -= gearSpeed * deltaTime;
        if (gearAnimationState < targetGear) gearAnimationState = targetGear;
    }
}

void Aircraft::applyPhysics(float deltaTime) {
    // Calculate speed in km/h for comparison with specs
    float speedKmh = speed * 3.6f;
    float maxSpeedMs = specs.maxSpeed / 3.6f;
    
    // Calculate stall speed (more realistic)
    float stallSpeedCurrent = specs.stallSpeed;
    if (flapsLevel > 0) {
        stallSpeedCurrent *= (1.0f - 0.08f * flapsLevel);  // Flaps reduce stall speed more
    }
    // Stall also depends on altitude (thinner air = higher stall speed)
    float altitudeFactor = 1.0f + (position.y / specs.maxAltitude) * 0.3f;
    stallSpeedCurrent *= altitudeFactor;
    
    stalling = !onGround && speedKmh < stallSpeedCurrent && position.y > 10.0f;
    
    // Throttle affects speed with smooth, arcade-friendly acceleration
    float targetSpeed = throttle * maxSpeedMs;
    
    // Improved drag model (less restrictive for fun gameplay)
    float dragFactor = dragCoefficient * 1.0f;  // Reduced base drag
    if (landingGearDown && !onGround) {
        dragFactor += 0.010f;  // Reduced gear drag
    }
    dragFactor += flapsLevel * 0.005f;  // Reduced flap drag
    
    // Altitude air density effects
    float altitudeDensity = std::exp(-position.y / 8500.0f);  // Exponential atmosphere
    
    // Calculate drag force more realistically
    float dragForce = 0.5f * dragFactor * altitudeDensity * speed * speed * 0.008f;
    
    if (onGround) {
        // Ground friction and braking - responsive
        if (brakeEngaged) {
            speed -= speed * 2.5f * deltaTime;  // Effective braking
        } else {
            // Smooth acceleration/deceleration on ground - arcade-friendly
            float accelFactor = specs.acceleration * 0.6f;
            speed += (targetSpeed - speed) * accelFactor * deltaTime;
        }
    } else {
        // Air dynamics - thrust vs drag - more forgiving
        if (throttle > 0.05f) {
            // Thrust acceleration - responsive
            float thrustForce = throttle * specs.acceleration * specs.weight * 1.2f;
            speed += (thrustForce / specs.weight) * deltaTime;
        }
        // Always apply drag, but less punishing
        speed -= dragForce * deltaTime * 0.7f;
    }
    
    // Clamp speed
    speed = std::max(0.0f, std::min(speed, maxSpeedMs * 1.1f));  // Allow slight overspeed
    
    // Flight controls scale with speed - very responsive across speed range
    // Minimum control authority even at low speeds for fun factor
    float controlAuthority = 0.4f + std::min(0.6f, speedKmh / (specs.stallSpeed * 1.8f));
    
    if (!onGround) {
        // Pitch control - smooth and responsive, great for loops
        float pitchRate = specs.turnRate * 5.5f * pitchInput * controlAuthority;
        rotation.x += pitchRate * deltaTime;
        rotation.x = std::clamp(rotation.x, -90.0f, 90.0f);  // Allow extreme pitch for loops
        
        // Roll control - very responsive for barrel rolls and aerobatics
        float rollRate = specs.turnRate * 10.0f * rollInput * controlAuthority;
        rotation.z += rollRate * deltaTime;
        rotation.z = std::clamp(rotation.z, -180.0f, 180.0f);  // Allow full inverted flight
        
        // Improved coordinated turn - natural banking from roll
        float bankInfluence = std::sin(rotation.z * DEG_TO_RAD) * speed / std::max(specs.maxSpeed / 3.6f, 10.0f);
        rotation.y += bankInfluence * specs.turnRate * 0.6f * deltaTime;
        
        // Yaw control - responsive for turning
        float yawRate = specs.turnRate * 2.5f * yawInput * controlAuthority;
        rotation.y += yawRate * deltaTime;
        
        // Normalize heading
        while (rotation.y < 0) rotation.y += 360.0f;
        while (rotation.y >= 360.0f) rotation.y -= 360.0f;
        
        // More forgiving and fun lift model - based on angle of attack, not just pitch input
        float angleOfAttack = rotation.x * DEG_TO_RAD;
        
        // Natural lift from airspeed - creates sustainable flight
        // Base lift coefficient varies with speed (more lift at higher speeds)
        float speedRatio = std::min(1.0f, speedKmh / (specs.stallSpeed * 1.5f));  // Normalized to stall speed
        float baseLift = 0.6f + 0.3f * speedRatio;  // Always some lift, more at higher speeds
        float liftCoeff = baseLift;
        
        // Additional lift from pitch angle when pointed up (natural aerodynamics)
        // Pitch up increases lift significantly
        if (angleOfAttack > 0.0f && angleOfAttack < 1.2f) {  // Wider optimal climb angle
            liftCoeff += (angleOfAttack / 1.2f) * 1.2f;  // Even stronger increase with pitch up
        }
        
        // User pitch input ADDS to lift strongly
        liftCoeff += std::max(0.0f, pitchInput) * 0.7f;  // Very strong pitch input response
        
        // Stall still reduces lift, but less dramatically for fun factor
        if (stalling) {
            liftCoeff *= 0.5f;  // More lift even when stalling
        }
        
        // Flaps improve lift substantially
        if (flapsLevel > 0) {
            liftCoeff *= (1.0f + 0.3f * flapsLevel);  // Even more effective flaps
        }
        
        // Calculate lift force - use a better formula that works at low speeds
        // The key is to not rely solely on speed^2, add a baseline force component
        float speedComponent = altitudeDensity * speed * speed * 0.15f;  // Increased scaling
        float baselineLifting = specs.weight * 0.35f * altitudeDensity;  // Much stronger baseline - allows takeoff
        float liftForce = liftCoeff * (speedComponent + baselineLifting);  // Combined approach
        
        // Gravity - weight always pulls down
        float weightForce = specs.weight * gravity;
        
        // Net vertical force: lift minus weight
        // Direct calculation without aggressive dampening
        float netVerticalForce = (liftForce - weightForce);
        
        // Apply vertical acceleration smoothly
        // Scale by a reasonable factor for more responsive lift
        verticalSpeed += (netVerticalForce / specs.weight) * deltaTime * 8.0f;
        
        // Throttle helps climb significantly - engines provide power
        verticalSpeed += throttle * specs.climbRate * 0.8f * deltaTime;
        
        // Limit vertical speed with generous margins for aerobatics
        verticalSpeed = std::clamp(verticalSpeed, -specs.climbRate * 3.0f, specs.climbRate * 2.5f);
    } else {
        // On ground physics
        verticalSpeed = 0;
        
        // Nose wheel steering - responsive but limited at low speed
        if (speed > 2.0f) {
            float steeringRate = std::min(45.0f, yawInput * 60.0f * (speed / maxSpeedMs));
            rotation.y += steeringRate * deltaTime;
        }
        
        // Gently level the aircraft on ground
        rotation.x *= 0.92f;
        rotation.z *= 0.92f;
    }
    
    // Update position
    float heading = rotation.y * DEG_TO_RAD;
    velocity.x = std::sin(heading) * speed;
    velocity.z = -std::cos(heading) * speed;  // Negative Z is forward
    velocity.y = verticalSpeed;
    
    position += velocity * deltaTime;
    
    // Altitude limits
    position.y = std::max(0.0f, std::min(specs.maxAltitude, position.y));
}

void Aircraft::checkGroundCollision() {
    float groundHeight = 0.0f;  // Flat ground for now
    float gearHeight = landingGearDown ? 3.0f : 1.5f;
    
    if (position.y <= groundHeight + gearHeight) {
        // Check if landing or crashing
        if (!onGround) {
            if (verticalSpeed < -10.0f || std::abs(rotation.z) > 15.0f || std::abs(rotation.x) > 20.0f) {
                // Hard landing / crash - reset for now
                // In full game, this would show crash animation
                std::cout << "Hard landing detected!" << std::endl;
            }
            onGround = true;
        }
        position.y = groundHeight + gearHeight;
        verticalSpeed = 0;
    } else {
        // Takeoff check
        if (onGround && speed > specs.stallSpeed / 3.6f * 0.8f && rotation.x > 5.0f) {
            onGround = false;
        }
    }
}

void Aircraft::setThrottle(float value) {
    throttle = std::max(0.0f, std::min(1.0f, value));
}

void Aircraft::setPitch(float value) {
    pitchInput = std::max(-1.0f, std::min(1.0f, value));
}

void Aircraft::setRoll(float value) {
    rollInput = std::max(-1.0f, std::min(1.0f, value));
}

void Aircraft::setYaw(float value) {
    yawInput = std::max(-1.0f, std::min(1.0f, value));
}

void Aircraft::setBrake(bool engaged) {
    brakeEngaged = engaged;
}

void Aircraft::toggleLandingGear() {
    if (!onGround) {  // Can only toggle gear in the air
        landingGearDown = !landingGearDown;
        std::cout << "Landing gear " << (landingGearDown ? "DOWN" : "UP") << std::endl;
    }
}

void Aircraft::adjustFlaps(int delta) {
    flapsLevel = std::max(0, std::min(3, flapsLevel + delta));
    std::cout << "Flaps level: " << flapsLevel << std::endl;
}

Vector3 Aircraft::getForwardVector() const {
    float pitch = rotation.x * DEG_TO_RAD;
    float yaw = rotation.y * DEG_TO_RAD;
    
    return Vector3(
        std::sin(yaw) * std::cos(pitch),
        -std::sin(pitch),
        -std::cos(yaw) * std::cos(pitch)
    );
}

Vector3 Aircraft::getUpVector() const {
    float pitch = rotation.x * DEG_TO_RAD;
    float yaw = rotation.y * DEG_TO_RAD;
    float roll = rotation.z * DEG_TO_RAD;
    
    // Simple up vector (doesn't account for roll properly, but good enough)
    Vector3 worldUp(0, 1, 0);
    Vector3 forward = getForwardVector();
    Vector3 right = Vector3::cross(forward, worldUp).normalized();
    Vector3 up = Vector3::cross(right, forward).normalized();
    
    // Apply roll
    return Vector3(
        up.x * std::cos(roll) + right.x * std::sin(roll),
        up.y * std::cos(roll) + right.y * std::sin(roll),
        up.z * std::cos(roll) + right.z * std::sin(roll)
    );
}

Vector3 Aircraft::getRightVector() const {
    Vector3 forward = getForwardVector();
    Vector3 up = getUpVector();
    return Vector3::cross(forward, up).normalized();
}
