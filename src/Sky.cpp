#include "Sky.h"
#include <cmath>
#include <cstdlib>

Sky::Sky() {
    setTimeOfDay(12.0f);  // Start at noon
    generateClouds();
}

Sky::~Sky() {}

void Sky::update(float deltaTime) {
    // Advance time if enabled
    if (daySpeed > 0) {
        timeOfDay += daySpeed * deltaTime;
        if (timeOfDay >= 24.0f) timeOfDay -= 24.0f;
        updateSkyColors();
    }
    
    // Move clouds
    updateClouds(deltaTime);
}

void Sky::setTimeOfDay(float hours) {
    timeOfDay = std::fmod(hours, 24.0f);
    if (timeOfDay < 0) timeOfDay += 24.0f;
    updateSkyColors();
}

void Sky::updateSkyColors() {
    // Calculate sun position based on time
    float sunAngle = (timeOfDay - 6.0f) / 12.0f * PI;  // 6am = sunrise, 6pm = sunset
    sunIntensity = std::max(0.0f, std::sin(sunAngle));
    
    // Sky colors based on time of day
    if (timeOfDay >= 6.0f && timeOfDay < 8.0f) {
        // Sunrise
        float t = (timeOfDay - 6.0f) / 2.0f;
        skyColorTop = Color(
            0.2f + t * 0.3f,
            0.3f + t * 0.4f,
            0.5f + t * 0.4f
        );
        skyColorHorizon = Color(
            0.9f - t * 0.3f,
            0.5f + t * 0.3f,
            0.3f + t * 0.4f
        );
        sunColor = Color(1.0f, 0.7f + t * 0.2f, 0.4f + t * 0.4f);
    }
    else if (timeOfDay >= 8.0f && timeOfDay < 17.0f) {
        // Daytime
        skyColorTop = Color(0.4f, 0.6f, 0.9f);
        skyColorHorizon = Color(0.6f, 0.75f, 0.9f);
        sunColor = Color(1.0f, 0.95f, 0.85f);
        
        // Adjust based on weather
        if (currentWeather == 1) {  // Cloudy
            skyColorTop = Color(0.5f, 0.55f, 0.6f);
            skyColorHorizon = Color(0.6f, 0.65f, 0.7f);
        } else if (currentWeather == 2) {  // Overcast
            skyColorTop = Color(0.4f, 0.42f, 0.45f);
            skyColorHorizon = Color(0.5f, 0.52f, 0.55f);
        }
    }
    else if (timeOfDay >= 17.0f && timeOfDay < 19.0f) {
        // Sunset
        float t = (timeOfDay - 17.0f) / 2.0f;
        skyColorTop = Color(
            0.4f - t * 0.2f,
            0.6f - t * 0.35f,
            0.9f - t * 0.5f
        );
        skyColorHorizon = Color(
            0.6f + t * 0.3f,
            0.6f - t * 0.2f,
            0.7f - t * 0.4f
        );
        sunColor = Color(1.0f, 0.6f - t * 0.2f, 0.3f);
    }
    else {
        // Night
        skyColorTop = Color(0.05f, 0.05f, 0.15f);
        skyColorHorizon = Color(0.1f, 0.1f, 0.2f);
        sunColor = Color(0.8f, 0.8f, 0.9f);  // Moon color
    }
    
    // Fog color matches horizon
    fogColor = skyColorHorizon;
    
    // Adjust fog based on weather
    if (currentWeather == 1) {
        fogDensity = 0.002f;
        fogStart = 300.0f;
        fogEnd = 3000.0f;
    } else if (currentWeather == 2) {
        fogDensity = 0.005f;
        fogStart = 100.0f;
        fogEnd = 1500.0f;
    } else {
        fogDensity = 0.001f;
        fogStart = 500.0f;
        fogEnd = 5000.0f;
    }
}

Vector3 Sky::getSunDirection() const {
    float sunAngle = (timeOfDay - 6.0f) / 12.0f * PI;
    
    return Vector3(
        0.3f,  // Slight east-west offset
        std::sin(sunAngle),
        -std::cos(sunAngle) * 0.5f
    ).normalized();
}

void Sky::generateClouds() {
    cloudPositions.clear();
    cloudSizes.clear();
    
    int cloudCount = 50 + currentWeather * 30;  // More clouds in bad weather
    
    for (int i = 0; i < cloudCount; i++) {
        float x = ((float)rand() / RAND_MAX - 0.5f) * 8000.0f;
        float y = 500.0f + ((float)rand() / RAND_MAX) * 500.0f;
        float z = ((float)rand() / RAND_MAX - 0.5f) * 8000.0f;
        
        cloudPositions.push_back(Vector3(x, y, z));
        
        float size = 100.0f + ((float)rand() / RAND_MAX) * 200.0f;
        cloudSizes.push_back(size);
    }
}

void Sky::updateClouds(float deltaTime) {
    // Move clouds slowly
    for (auto& pos : cloudPositions) {
        pos.x += cloudSpeed * deltaTime;
        
        // Wrap around
        if (pos.x > 4000.0f) pos.x -= 8000.0f;
    }
}

void Sky::setWeather(int weather) {
    currentWeather = std::max(0, std::min(2, weather));
    updateSkyColors();
    generateClouds();
}
