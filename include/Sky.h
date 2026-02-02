#pragma once

#include "Types.h"
#include <vector>

class Sky {
public:
    Sky();
    ~Sky();
    
    void update(float deltaTime);
    
    // Time of day (0-24 hours)
    void setTimeOfDay(float hours);
    float getTimeOfDay() const { return timeOfDay; }
    
    // Sky colors
    Color getSkyColorTop() const { return skyColorTop; }
    Color getSkyColorHorizon() const { return skyColorHorizon; }
    Color getSunColor() const { return sunColor; }
    Color getFogColor() const { return fogColor; }
    
    // Sun position
    Vector3 getSunDirection() const;
    float getSunIntensity() const { return sunIntensity; }
    
    // Clouds
    const std::vector<Vector3>& getCloudPositions() const { return cloudPositions; }
    const std::vector<float>& getCloudSizes() const { return cloudSizes; }
    
    // Fog
    float getFogDensity() const { return fogDensity; }
    float getFogStart() const { return fogStart; }
    float getFogEnd() const { return fogEnd; }
    
    // Weather
    void setWeather(int weather); // 0=Clear, 1=Cloudy, 2=Overcast
    int getWeather() const { return currentWeather; }
    
private:
    void updateSkyColors();
    void generateClouds();
    void updateClouds(float deltaTime);
    
    float timeOfDay = 12.0f;  // Default to noon
    float daySpeed = 0.0f;     // Hours per second (0 = frozen time)
    
    Color skyColorTop;
    Color skyColorHorizon;
    Color sunColor;
    Color fogColor;
    float sunIntensity = 1.0f;
    
    // Fog settings
    float fogDensity = 0.001f;
    float fogStart = 500.0f;
    float fogEnd = 5000.0f;
    
    // Clouds
    std::vector<Vector3> cloudPositions;
    std::vector<float> cloudSizes;
    float cloudSpeed = 5.0f;
    
    int currentWeather = 0;
};
