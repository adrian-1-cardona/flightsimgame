#pragma once

#include "Types.h"
#include <string>
#include <vector>

class Renderer;

class LoadingScreen {
public:
    LoadingScreen();
    ~LoadingScreen();
    
    void update(float deltaTime);
    void render(Renderer* renderer);
    
    // Progress management
    void setProgress(float progress);  // 0.0 to 1.0
    float getProgress() const { return loadProgress; }
    void setLoadingText(const std::string& text);
    void setTip(const std::string& tip);
    
    // Animation
    void startAnimation();
    bool isComplete() const { return loadProgress >= 1.0f; }
    
private:
    void renderBackground(Renderer* renderer);
    void renderLogo(Renderer* renderer);
    void renderProgressBar(Renderer* renderer);
    void renderLoadingText(Renderer* renderer);
    void renderTips(Renderer* renderer);
    void renderPlaneAnimation(Renderer* renderer);
    
    float loadProgress = 0.0f;
    std::string loadingText = "Loading...";
    std::string currentTip;
    
    // Animation
    float animationTime = 0.0f;
    float planeX = 0.0f;
    float fadeAlpha = 1.0f;
    
    // Tips rotation
    std::vector<std::string> tips;
    int currentTipIndex = 0;
    float tipTimer = 0.0f;
    float tipDuration = 4.0f;
};
