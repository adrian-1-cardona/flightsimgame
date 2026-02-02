#pragma once

#include "Types.h"
#include <SDL2/SDL.h>
#include <string>
#include <vector>

class Camera;
class Aircraft;
class Terrain;
class Sky;

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    bool initialize(SDL_Window* window, int width, int height);
    void shutdown();
    
    void beginFrame();
    void endFrame();
    
    // Rendering functions
    void renderAircraft(const Aircraft* aircraft, const Camera* camera);
    void renderTerrain(const Terrain* terrain, const Camera* camera);
    void renderSky(const Sky* sky, const Camera* camera);
    void renderHUD(const Aircraft* aircraft);
    void renderMinimap(const Aircraft* aircraft, const Terrain* terrain);
    
    // UI rendering
    void renderText(const std::string& text, float x, float y, float scale, const Color& color);
    void renderRect(float x, float y, float width, float height, const Color& color, bool filled = true);
    void renderCircle(float x, float y, float radius, const Color& color, bool filled = true);
    void renderProgressBar(float x, float y, float width, float height, float progress, 
                          const Color& bgColor, const Color& fillColor);
    
    // 3D primitives
    void renderCube(const Vector3& position, const Vector3& size, const Color& color);
    void renderCylinder(const Vector3& position, float radius, float height, const Color& color);
    void renderSphere(const Vector3& position, float radius, const Color& color);
    void renderPlane3D(const Vector3& position, const Vector3& rotation, 
                       float wingspan, float length, const Color& primaryColor, const Color& secondaryColor,
                       float gearState = 1.0f, float flapsState = 0.0f);
    
    // Utility
    void setViewport(int x, int y, int width, int height);
    void setClearColor(const Color& color);
    void setProjectionMatrix(float fov, float aspect, float nearPlane, float farPlane);
    void setViewMatrix(const Vector3& eye, const Vector3& target, const Vector3& up);
    
    int getWidth() const { return screenWidth; }
    int getHeight() const { return screenHeight; }
    
private:
    void initOpenGL();
    void setupMatrices();
    void drawCharacter(char c, float x, float y, float scale);
    
    SDL_Window* window = nullptr;
    SDL_GLContext glContext = nullptr;
    
    int screenWidth = 1920;
    int screenHeight = 1080;
    
    Color clearColor = Color::SkyBlue();
};
