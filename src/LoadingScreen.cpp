#include "LoadingScreen.h"
#include "Renderer.h"
#include <cmath>

LoadingScreen::LoadingScreen() {
    // Initialize tips
    tips = {
        "Use the LEFT STICK to control pitch and roll",
        "Press G to toggle landing gear",
        "Monitor your airspeed to avoid stalling",
        "Use flaps (F/V) for slower takeoffs and landings",
        "Connect a Bluetooth controller for the best experience",
        "Press C to cycle through camera views",
        "The F-22 Raptor is the fastest aircraft available",
        "Keep your nose up during takeoff for better climb",
        "Reduce throttle before landing to slow down"
    };
    
    currentTip = tips[0];
}

LoadingScreen::~LoadingScreen() {}

void LoadingScreen::update(float deltaTime) {
    animationTime += deltaTime;
    
    // Animate loading plane
    planeX = std::sin(animationTime * 2.0f) * 100.0f;
    
    // Rotate tips
    tipTimer += deltaTime;
    if (tipTimer >= tipDuration) {
        tipTimer = 0.0f;
        currentTipIndex = (currentTipIndex + 1) % tips.size();
        currentTip = tips[currentTipIndex];
    }
    
    // Fade out when complete
    if (loadProgress >= 1.0f) {
        fadeAlpha -= deltaTime * 2.0f;
        if (fadeAlpha < 0.0f) fadeAlpha = 0.0f;
    }
}

void LoadingScreen::render(Renderer* renderer) {
    if (fadeAlpha <= 0.0f) return;
    
    renderBackground(renderer);
    renderLogo(renderer);
    renderPlaneAnimation(renderer);
    renderProgressBar(renderer);
    renderLoadingText(renderer);
    renderTips(renderer);
}

void LoadingScreen::renderBackground(Renderer* renderer) {
    int w = renderer->getWidth();
    int h = renderer->getHeight();
    
    // Dark blue gradient background
    renderer->renderRect(0, 0, w, h, Color(0.05f, 0.08f, 0.15f, fadeAlpha), true);
    
    // Add some subtle pattern/stars
    float starBrightness = 0.3f + 0.1f * std::sin(animationTime * 0.5f);
    for (int i = 0; i < 50; i++) {
        float x = ((i * 73) % w);
        float y = ((i * 137) % h);
        float size = 1.0f + (i % 3);
        float twinkle = 0.5f + 0.5f * std::sin(animationTime * 3.0f + i);
        renderer->renderCircle(x, y, size, 
            Color(1.0f, 1.0f, 1.0f, starBrightness * twinkle * fadeAlpha), true);
    }
}

void LoadingScreen::renderLogo(Renderer* renderer) {
    int w = renderer->getWidth();
    
    // Title with glow effect
    float glowPulse = 0.8f + 0.2f * std::sin(animationTime * 2.0f);
    
    // Glow behind text
    renderer->renderText("FLIGHT SIMULATOR PRO", w/2 - 183, 148, 2.0f, 
        Color(0.2f, 0.5f, 1.0f, 0.3f * glowPulse * fadeAlpha));
    
    // Main title
    renderer->renderText("FLIGHT SIMULATOR PRO", w/2 - 180, 150, 2.0f, 
        Color(0.3f, 0.7f, 1.0f, fadeAlpha));
    
    // Subtitle
    renderer->renderText("The Ultimate Flying Experience", w/2 - 150, 200, 1.0f, 
        Color(0.6f, 0.6f, 0.7f, fadeAlpha));
}

void LoadingScreen::renderPlaneAnimation(Renderer* renderer) {
    int w = renderer->getWidth();
    int h = renderer->getHeight();
    
    float centerX = w / 2.0f + planeX;
    float centerY = h * 0.45f + std::sin(animationTime * 3.0f) * 10.0f;
    
    // Draw animated plane silhouette
    float rotation = std::sin(animationTime * 2.0f) * 10.0f;
    
    // Fuselage
    float fuselageLength = 60.0f;
    float fuselageWidth = 8.0f;
    renderer->renderRect(centerX - fuselageWidth/2, centerY - fuselageLength/2, 
        fuselageWidth, fuselageLength, Color(0.7f, 0.7f, 0.8f, fadeAlpha), true);
    
    // Wings
    float wingspan = 80.0f;
    float wingChord = 15.0f;
    renderer->renderRect(centerX - wingspan/2, centerY - 5, 
        wingspan, wingChord, Color(0.5f, 0.6f, 0.8f, fadeAlpha), true);
    
    // Tail
    float tailWidth = 30.0f;
    renderer->renderRect(centerX - tailWidth/2, centerY + fuselageLength/2 - 10, 
        tailWidth, 8.0f, Color(0.5f, 0.6f, 0.8f, fadeAlpha), true);
    
    // Vertical stabilizer
    renderer->renderRect(centerX - 3, centerY + 15, 
        6, 20.0f, Color(0.7f, 0.7f, 0.8f, fadeAlpha), true);
    
    // Engine glow/trail
    float trailAlpha = 0.5f + 0.3f * std::sin(animationTime * 10.0f);
    for (int i = 0; i < 3; i++) {
        float trailX = centerX - wingspan/2 + 15 + i * 25;
        float trailY = centerY + 5;
        renderer->renderCircle(trailX, trailY + 10 + i * 3, 4.0f - i, 
            Color(1.0f, 0.6f, 0.2f, trailAlpha * fadeAlpha * (1.0f - i * 0.3f)), true);
    }
}

void LoadingScreen::renderProgressBar(Renderer* renderer) {
    int w = renderer->getWidth();
    int h = renderer->getHeight();
    
    float barWidth = 400.0f;
    float barHeight = 8.0f;
    float barX = (w - barWidth) / 2.0f;
    float barY = h * 0.65f;
    
    // Progress bar background
    renderer->renderRect(barX, barY, barWidth, barHeight, 
        Color(0.2f, 0.2f, 0.3f, fadeAlpha), true);
    
    // Progress bar fill with gradient effect
    float fillWidth = barWidth * loadProgress;
    if (fillWidth > 0) {
        // Main fill
        renderer->renderRect(barX, barY, fillWidth, barHeight, 
            Color(0.2f, 0.6f, 1.0f, fadeAlpha), true);
        
        // Highlight at the end
        float highlightWidth = std::min(20.0f, fillWidth);
        float highlightX = barX + fillWidth - highlightWidth;
        float pulse = 0.5f + 0.5f * std::sin(animationTime * 5.0f);
        renderer->renderRect(highlightX, barY, highlightWidth, barHeight, 
            Color(0.5f, 0.8f, 1.0f, pulse * fadeAlpha), true);
    }
    
    // Border
    renderer->renderRect(barX, barY, barWidth, barHeight, 
        Color(0.4f, 0.5f, 0.7f, fadeAlpha), false);
    
    // Percentage
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%.0f%%", loadProgress * 100.0f);
    renderer->renderText(buffer, w/2 - 15, barY + 20, 1.0f, 
        Color(0.7f, 0.7f, 0.8f, fadeAlpha));
}

void LoadingScreen::renderLoadingText(Renderer* renderer) {
    int w = renderer->getWidth();
    int h = renderer->getHeight();
    
    // Loading status text with animated dots
    int dots = (int)(animationTime * 2) % 4;
    std::string loadingWithDots = loadingText;
    for (int i = 0; i < dots; i++) {
        loadingWithDots += ".";
    }
    
    renderer->renderText(loadingWithDots.c_str(), w/2 - 80, h * 0.58f, 1.0f, 
        Color(0.8f, 0.8f, 0.9f, fadeAlpha));
}

void LoadingScreen::renderTips(Renderer* renderer) {
    int w = renderer->getWidth();
    int h = renderer->getHeight();
    
    // Tip label
    renderer->renderText("TIP:", w/2 - 200, h * 0.78f, 0.9f, 
        Color(0.4f, 0.6f, 0.8f, fadeAlpha));
    
    // Current tip with fade effect
    float tipFade = 1.0f;
    if (tipTimer < 0.5f) {
        tipFade = tipTimer * 2.0f;  // Fade in
    } else if (tipTimer > tipDuration - 0.5f) {
        tipFade = (tipDuration - tipTimer) * 2.0f;  // Fade out
    }
    
    renderer->renderText(currentTip.c_str(), w/2 - 200, h * 0.82f, 0.9f, 
        Color(0.7f, 0.7f, 0.8f, tipFade * fadeAlpha));
}

void LoadingScreen::setProgress(float progress) {
    loadProgress = std::max(0.0f, std::min(1.0f, progress));
}

void LoadingScreen::setLoadingText(const std::string& text) {
    loadingText = text;
}

void LoadingScreen::setTip(const std::string& tip) {
    currentTip = tip;
}

void LoadingScreen::startAnimation() {
    animationTime = 0.0f;
    fadeAlpha = 1.0f;
    loadProgress = 0.0f;
}
