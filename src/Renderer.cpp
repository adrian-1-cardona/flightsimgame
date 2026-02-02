#include "Renderer.h"
#include "Camera.h"
#include "Aircraft.h"
#include "Terrain.h"
#include "Sky.h"

#include <cmath>
#include <iostream>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

Renderer::Renderer() {}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::initialize(SDL_Window* win, int width, int height) {
    window = win;
    screenWidth = width;
    screenHeight = height;
    
    initOpenGL();
    
    std::cout << "Renderer initialized: " << screenWidth << "x" << screenHeight << std::endl;
    return true;
}

void Renderer::initOpenGL() {
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    // Enable smooth shading
    glShadeModel(GL_SMOOTH);
    
    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // Set up light
    GLfloat lightPosition[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    GLfloat lightAmbient[] = { 0.3f, 0.3f, 0.35f, 1.0f };
    GLfloat lightDiffuse[] = { 0.9f, 0.9f, 0.85f, 1.0f };
    GLfloat lightSpecular[] = { 1.0f, 1.0f, 0.95f, 1.0f };
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    
    // Enable color material
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Enable multisampling
    glEnable(GL_MULTISAMPLE);
    
    // Set viewport
    glViewport(0, 0, screenWidth, screenHeight);
    
    // Set clear color
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
}

void Renderer::shutdown() {
    // Cleanup OpenGL resources if needed
}

void Renderer::beginFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endFrame() {
    glFlush();
}

void Renderer::setViewport(int x, int y, int width, int height) {
    screenWidth = width;
    screenHeight = height;
    glViewport(x, y, width, height);
}

void Renderer::setClearColor(const Color& color) {
    clearColor = color;
    glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer::setProjectionMatrix(float fov, float aspect, float nearPlane, float farPlane) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, aspect, nearPlane, farPlane);
    glMatrixMode(GL_MODELVIEW);
}

void Renderer::setViewMatrix(const Vector3& eye, const Vector3& target, const Vector3& up) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye.x, eye.y, eye.z,
              target.x, target.y, target.z,
              up.x, up.y, up.z);
}

void Renderer::renderText(const std::string& text, float x, float y, float scale, const Color& color) {
    // Switch to 2D rendering
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, screenWidth, screenHeight, 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    glColor4f(color.r, color.g, color.b, color.a);
    
    float charWidth = 10.0f * scale;
    float charHeight = 16.0f * scale;
    float cursorX = x;
    
    for (char c : text) {
        if (c == '\n') {
            cursorX = x;
            y += charHeight * 1.2f;
            continue;
        }
        drawCharacter(c, cursorX, y, scale);
        cursorX += charWidth;
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void Renderer::drawCharacter(char c, float x, float y, float scale) {
    // Simple bitmap font rendering using lines
    float w = 8.0f * scale;
    float h = 14.0f * scale;
    
    glBegin(GL_LINES);
    
    // Define simple line-based characters
    switch (c) {
        case 'A': case 'a':
            glVertex2f(x, y + h); glVertex2f(x + w/2, y);
            glVertex2f(x + w/2, y); glVertex2f(x + w, y + h);
            glVertex2f(x + w*0.2f, y + h*0.6f); glVertex2f(x + w*0.8f, y + h*0.6f);
            break;
        case 'B': case 'b':
            glVertex2f(x, y); glVertex2f(x, y + h);
            glVertex2f(x, y); glVertex2f(x + w*0.7f, y);
            glVertex2f(x + w*0.7f, y); glVertex2f(x + w, y + h*0.2f);
            glVertex2f(x + w, y + h*0.2f); glVertex2f(x + w*0.7f, y + h*0.5f);
            glVertex2f(x, y + h*0.5f); glVertex2f(x + w*0.7f, y + h*0.5f);
            glVertex2f(x + w*0.7f, y + h*0.5f); glVertex2f(x + w, y + h*0.8f);
            glVertex2f(x + w, y + h*0.8f); glVertex2f(x + w*0.7f, y + h);
            glVertex2f(x, y + h); glVertex2f(x + w*0.7f, y + h);
            break;
        case 'C': case 'c':
            glVertex2f(x + w, y + h*0.2f); glVertex2f(x + w*0.5f, y);
            glVertex2f(x + w*0.5f, y); glVertex2f(x, y + h*0.3f);
            glVertex2f(x, y + h*0.3f); glVertex2f(x, y + h*0.7f);
            glVertex2f(x, y + h*0.7f); glVertex2f(x + w*0.5f, y + h);
            glVertex2f(x + w*0.5f, y + h); glVertex2f(x + w, y + h*0.8f);
            break;
        case 'D': case 'd':
            glVertex2f(x, y); glVertex2f(x, y + h);
            glVertex2f(x, y); glVertex2f(x + w*0.6f, y);
            glVertex2f(x + w*0.6f, y); glVertex2f(x + w, y + h*0.3f);
            glVertex2f(x + w, y + h*0.3f); glVertex2f(x + w, y + h*0.7f);
            glVertex2f(x + w, y + h*0.7f); glVertex2f(x + w*0.6f, y + h);
            glVertex2f(x + w*0.6f, y + h); glVertex2f(x, y + h);
            break;
        case 'E': case 'e':
            glVertex2f(x, y); glVertex2f(x, y + h);
            glVertex2f(x, y); glVertex2f(x + w, y);
            glVertex2f(x, y + h*0.5f); glVertex2f(x + w*0.7f, y + h*0.5f);
            glVertex2f(x, y + h); glVertex2f(x + w, y + h);
            break;
        case 'F': case 'f':
            glVertex2f(x, y); glVertex2f(x, y + h);
            glVertex2f(x, y); glVertex2f(x + w, y);
            glVertex2f(x, y + h*0.5f); glVertex2f(x + w*0.7f, y + h*0.5f);
            break;
        case 'G': case 'g':
            glVertex2f(x + w, y + h*0.2f); glVertex2f(x + w*0.5f, y);
            glVertex2f(x + w*0.5f, y); glVertex2f(x, y + h*0.3f);
            glVertex2f(x, y + h*0.3f); glVertex2f(x, y + h*0.7f);
            glVertex2f(x, y + h*0.7f); glVertex2f(x + w*0.5f, y + h);
            glVertex2f(x + w*0.5f, y + h); glVertex2f(x + w, y + h*0.7f);
            glVertex2f(x + w, y + h*0.7f); glVertex2f(x + w, y + h*0.5f);
            glVertex2f(x + w, y + h*0.5f); glVertex2f(x + w*0.5f, y + h*0.5f);
            break;
        case 'H': case 'h':
            glVertex2f(x, y); glVertex2f(x, y + h);
            glVertex2f(x + w, y); glVertex2f(x + w, y + h);
            glVertex2f(x, y + h*0.5f); glVertex2f(x + w, y + h*0.5f);
            break;
        case 'I': case 'i':
            glVertex2f(x + w*0.3f, y); glVertex2f(x + w*0.7f, y);
            glVertex2f(x + w*0.5f, y); glVertex2f(x + w*0.5f, y + h);
            glVertex2f(x + w*0.3f, y + h); glVertex2f(x + w*0.7f, y + h);
            break;
        case 'L': case 'l':
            glVertex2f(x, y); glVertex2f(x, y + h);
            glVertex2f(x, y + h); glVertex2f(x + w, y + h);
            break;
        case 'M': case 'm':
            glVertex2f(x, y + h); glVertex2f(x, y);
            glVertex2f(x, y); glVertex2f(x + w*0.5f, y + h*0.4f);
            glVertex2f(x + w*0.5f, y + h*0.4f); glVertex2f(x + w, y);
            glVertex2f(x + w, y); glVertex2f(x + w, y + h);
            break;
        case 'N': case 'n':
            glVertex2f(x, y + h); glVertex2f(x, y);
            glVertex2f(x, y); glVertex2f(x + w, y + h);
            glVertex2f(x + w, y + h); glVertex2f(x + w, y);
            break;
        case 'O': case 'o':
            glVertex2f(x + w*0.3f, y); glVertex2f(x + w*0.7f, y);
            glVertex2f(x + w*0.7f, y); glVertex2f(x + w, y + h*0.3f);
            glVertex2f(x + w, y + h*0.3f); glVertex2f(x + w, y + h*0.7f);
            glVertex2f(x + w, y + h*0.7f); glVertex2f(x + w*0.7f, y + h);
            glVertex2f(x + w*0.7f, y + h); glVertex2f(x + w*0.3f, y + h);
            glVertex2f(x + w*0.3f, y + h); glVertex2f(x, y + h*0.7f);
            glVertex2f(x, y + h*0.7f); glVertex2f(x, y + h*0.3f);
            glVertex2f(x, y + h*0.3f); glVertex2f(x + w*0.3f, y);
            break;
        case 'P': case 'p':
            glVertex2f(x, y); glVertex2f(x, y + h);
            glVertex2f(x, y); glVertex2f(x + w*0.7f, y);
            glVertex2f(x + w*0.7f, y); glVertex2f(x + w, y + h*0.15f);
            glVertex2f(x + w, y + h*0.15f); glVertex2f(x + w, y + h*0.35f);
            glVertex2f(x + w, y + h*0.35f); glVertex2f(x + w*0.7f, y + h*0.5f);
            glVertex2f(x + w*0.7f, y + h*0.5f); glVertex2f(x, y + h*0.5f);
            break;
        case 'R': case 'r':
            glVertex2f(x, y); glVertex2f(x, y + h);
            glVertex2f(x, y); glVertex2f(x + w*0.7f, y);
            glVertex2f(x + w*0.7f, y); glVertex2f(x + w, y + h*0.15f);
            glVertex2f(x + w, y + h*0.15f); glVertex2f(x + w, y + h*0.35f);
            glVertex2f(x + w, y + h*0.35f); glVertex2f(x + w*0.7f, y + h*0.5f);
            glVertex2f(x + w*0.7f, y + h*0.5f); glVertex2f(x, y + h*0.5f);
            glVertex2f(x + w*0.5f, y + h*0.5f); glVertex2f(x + w, y + h);
            break;
        case 'S': case 's':
            glVertex2f(x + w, y + h*0.15f); glVertex2f(x + w*0.7f, y);
            glVertex2f(x + w*0.7f, y); glVertex2f(x + w*0.3f, y);
            glVertex2f(x + w*0.3f, y); glVertex2f(x, y + h*0.15f);
            glVertex2f(x, y + h*0.15f); glVertex2f(x, y + h*0.35f);
            glVertex2f(x, y + h*0.35f); glVertex2f(x + w*0.3f, y + h*0.5f);
            glVertex2f(x + w*0.3f, y + h*0.5f); glVertex2f(x + w*0.7f, y + h*0.5f);
            glVertex2f(x + w*0.7f, y + h*0.5f); glVertex2f(x + w, y + h*0.65f);
            glVertex2f(x + w, y + h*0.65f); glVertex2f(x + w, y + h*0.85f);
            glVertex2f(x + w, y + h*0.85f); glVertex2f(x + w*0.7f, y + h);
            glVertex2f(x + w*0.7f, y + h); glVertex2f(x + w*0.3f, y + h);
            glVertex2f(x + w*0.3f, y + h); glVertex2f(x, y + h*0.85f);
            break;
        case 'T': case 't':
            glVertex2f(x, y); glVertex2f(x + w, y);
            glVertex2f(x + w*0.5f, y); glVertex2f(x + w*0.5f, y + h);
            break;
        case 'U': case 'u':
            glVertex2f(x, y); glVertex2f(x, y + h*0.7f);
            glVertex2f(x, y + h*0.7f); glVertex2f(x + w*0.3f, y + h);
            glVertex2f(x + w*0.3f, y + h); glVertex2f(x + w*0.7f, y + h);
            glVertex2f(x + w*0.7f, y + h); glVertex2f(x + w, y + h*0.7f);
            glVertex2f(x + w, y + h*0.7f); glVertex2f(x + w, y);
            break;
        case 'V': case 'v':
            glVertex2f(x, y); glVertex2f(x + w*0.5f, y + h);
            glVertex2f(x + w*0.5f, y + h); glVertex2f(x + w, y);
            break;
        case 'W': case 'w':
            glVertex2f(x, y); glVertex2f(x + w*0.25f, y + h);
            glVertex2f(x + w*0.25f, y + h); glVertex2f(x + w*0.5f, y + h*0.6f);
            glVertex2f(x + w*0.5f, y + h*0.6f); glVertex2f(x + w*0.75f, y + h);
            glVertex2f(x + w*0.75f, y + h); glVertex2f(x + w, y);
            break;
        case 'X': case 'x':
            glVertex2f(x, y); glVertex2f(x + w, y + h);
            glVertex2f(x + w, y); glVertex2f(x, y + h);
            break;
        case 'Y': case 'y':
            glVertex2f(x, y); glVertex2f(x + w*0.5f, y + h*0.5f);
            glVertex2f(x + w, y); glVertex2f(x + w*0.5f, y + h*0.5f);
            glVertex2f(x + w*0.5f, y + h*0.5f); glVertex2f(x + w*0.5f, y + h);
            break;
        case 'Z': case 'z':
            glVertex2f(x, y); glVertex2f(x + w, y);
            glVertex2f(x + w, y); glVertex2f(x, y + h);
            glVertex2f(x, y + h); glVertex2f(x + w, y + h);
            break;
        case '0':
            glVertex2f(x + w*0.3f, y); glVertex2f(x + w*0.7f, y);
            glVertex2f(x + w*0.7f, y); glVertex2f(x + w, y + h*0.2f);
            glVertex2f(x + w, y + h*0.2f); glVertex2f(x + w, y + h*0.8f);
            glVertex2f(x + w, y + h*0.8f); glVertex2f(x + w*0.7f, y + h);
            glVertex2f(x + w*0.7f, y + h); glVertex2f(x + w*0.3f, y + h);
            glVertex2f(x + w*0.3f, y + h); glVertex2f(x, y + h*0.8f);
            glVertex2f(x, y + h*0.8f); glVertex2f(x, y + h*0.2f);
            glVertex2f(x, y + h*0.2f); glVertex2f(x + w*0.3f, y);
            break;
        case '1':
            glVertex2f(x + w*0.3f, y + h*0.2f); glVertex2f(x + w*0.5f, y);
            glVertex2f(x + w*0.5f, y); glVertex2f(x + w*0.5f, y + h);
            glVertex2f(x + w*0.2f, y + h); glVertex2f(x + w*0.8f, y + h);
            break;
        case '2':
            glVertex2f(x, y + h*0.2f); glVertex2f(x + w*0.3f, y);
            glVertex2f(x + w*0.3f, y); glVertex2f(x + w*0.7f, y);
            glVertex2f(x + w*0.7f, y); glVertex2f(x + w, y + h*0.2f);
            glVertex2f(x + w, y + h*0.2f); glVertex2f(x + w, y + h*0.4f);
            glVertex2f(x + w, y + h*0.4f); glVertex2f(x, y + h);
            glVertex2f(x, y + h); glVertex2f(x + w, y + h);
            break;
        case '3':
            glVertex2f(x, y + h*0.15f); glVertex2f(x + w*0.3f, y);
            glVertex2f(x + w*0.3f, y); glVertex2f(x + w*0.7f, y);
            glVertex2f(x + w*0.7f, y); glVertex2f(x + w, y + h*0.15f);
            glVertex2f(x + w, y + h*0.15f); glVertex2f(x + w, y + h*0.4f);
            glVertex2f(x + w, y + h*0.4f); glVertex2f(x + w*0.5f, y + h*0.5f);
            glVertex2f(x + w*0.5f, y + h*0.5f); glVertex2f(x + w, y + h*0.6f);
            glVertex2f(x + w, y + h*0.6f); glVertex2f(x + w, y + h*0.85f);
            glVertex2f(x + w, y + h*0.85f); glVertex2f(x + w*0.7f, y + h);
            glVertex2f(x + w*0.7f, y + h); glVertex2f(x + w*0.3f, y + h);
            glVertex2f(x + w*0.3f, y + h); glVertex2f(x, y + h*0.85f);
            break;
        case '4':
            glVertex2f(x + w*0.7f, y); glVertex2f(x + w*0.7f, y + h);
            glVertex2f(x + w*0.7f, y); glVertex2f(x, y + h*0.6f);
            glVertex2f(x, y + h*0.6f); glVertex2f(x + w, y + h*0.6f);
            break;
        case '5':
            glVertex2f(x + w, y); glVertex2f(x, y);
            glVertex2f(x, y); glVertex2f(x, y + h*0.45f);
            glVertex2f(x, y + h*0.45f); glVertex2f(x + w*0.7f, y + h*0.45f);
            glVertex2f(x + w*0.7f, y + h*0.45f); glVertex2f(x + w, y + h*0.6f);
            glVertex2f(x + w, y + h*0.6f); glVertex2f(x + w, y + h*0.85f);
            glVertex2f(x + w, y + h*0.85f); glVertex2f(x + w*0.7f, y + h);
            glVertex2f(x + w*0.7f, y + h); glVertex2f(x + w*0.3f, y + h);
            glVertex2f(x + w*0.3f, y + h); glVertex2f(x, y + h*0.85f);
            break;
        case '6':
            glVertex2f(x + w*0.7f, y); glVertex2f(x + w*0.3f, y);
            glVertex2f(x + w*0.3f, y); glVertex2f(x, y + h*0.2f);
            glVertex2f(x, y + h*0.2f); glVertex2f(x, y + h*0.8f);
            glVertex2f(x, y + h*0.8f); glVertex2f(x + w*0.3f, y + h);
            glVertex2f(x + w*0.3f, y + h); glVertex2f(x + w*0.7f, y + h);
            glVertex2f(x + w*0.7f, y + h); glVertex2f(x + w, y + h*0.8f);
            glVertex2f(x + w, y + h*0.8f); glVertex2f(x + w, y + h*0.6f);
            glVertex2f(x + w, y + h*0.6f); glVertex2f(x + w*0.7f, y + h*0.5f);
            glVertex2f(x + w*0.7f, y + h*0.5f); glVertex2f(x, y + h*0.5f);
            break;
        case '7':
            glVertex2f(x, y); glVertex2f(x + w, y);
            glVertex2f(x + w, y); glVertex2f(x + w*0.3f, y + h);
            break;
        case '8':
            glVertex2f(x + w*0.3f, y); glVertex2f(x + w*0.7f, y);
            glVertex2f(x + w*0.7f, y); glVertex2f(x + w, y + h*0.15f);
            glVertex2f(x + w, y + h*0.15f); glVertex2f(x + w, y + h*0.35f);
            glVertex2f(x + w, y + h*0.35f); glVertex2f(x + w*0.7f, y + h*0.5f);
            glVertex2f(x + w*0.7f, y + h*0.5f); glVertex2f(x + w*0.3f, y + h*0.5f);
            glVertex2f(x + w*0.3f, y + h*0.5f); glVertex2f(x, y + h*0.35f);
            glVertex2f(x, y + h*0.35f); glVertex2f(x, y + h*0.15f);
            glVertex2f(x, y + h*0.15f); glVertex2f(x + w*0.3f, y);
            glVertex2f(x + w*0.3f, y + h*0.5f); glVertex2f(x, y + h*0.65f);
            glVertex2f(x, y + h*0.65f); glVertex2f(x, y + h*0.85f);
            glVertex2f(x, y + h*0.85f); glVertex2f(x + w*0.3f, y + h);
            glVertex2f(x + w*0.3f, y + h); glVertex2f(x + w*0.7f, y + h);
            glVertex2f(x + w*0.7f, y + h); glVertex2f(x + w, y + h*0.85f);
            glVertex2f(x + w, y + h*0.85f); glVertex2f(x + w, y + h*0.65f);
            glVertex2f(x + w, y + h*0.65f); glVertex2f(x + w*0.7f, y + h*0.5f);
            break;
        case '9':
            glVertex2f(x + w, y + h*0.5f); glVertex2f(x + w*0.3f, y + h*0.5f);
            glVertex2f(x + w*0.3f, y + h*0.5f); glVertex2f(x, y + h*0.35f);
            glVertex2f(x, y + h*0.35f); glVertex2f(x, y + h*0.15f);
            glVertex2f(x, y + h*0.15f); glVertex2f(x + w*0.3f, y);
            glVertex2f(x + w*0.3f, y); glVertex2f(x + w*0.7f, y);
            glVertex2f(x + w*0.7f, y); glVertex2f(x + w, y + h*0.15f);
            glVertex2f(x + w, y + h*0.15f); glVertex2f(x + w, y + h*0.8f);
            glVertex2f(x + w, y + h*0.8f); glVertex2f(x + w*0.7f, y + h);
            glVertex2f(x + w*0.7f, y + h); glVertex2f(x + w*0.3f, y + h);
            break;
        case '.':
            glVertex2f(x + w*0.4f, y + h*0.9f); glVertex2f(x + w*0.6f, y + h*0.9f);
            glVertex2f(x + w*0.6f, y + h*0.9f); glVertex2f(x + w*0.6f, y + h);
            glVertex2f(x + w*0.6f, y + h); glVertex2f(x + w*0.4f, y + h);
            glVertex2f(x + w*0.4f, y + h); glVertex2f(x + w*0.4f, y + h*0.9f);
            break;
        case ':':
            glVertex2f(x + w*0.4f, y + h*0.25f); glVertex2f(x + w*0.6f, y + h*0.25f);
            glVertex2f(x + w*0.6f, y + h*0.25f); glVertex2f(x + w*0.6f, y + h*0.35f);
            glVertex2f(x + w*0.6f, y + h*0.35f); glVertex2f(x + w*0.4f, y + h*0.35f);
            glVertex2f(x + w*0.4f, y + h*0.35f); glVertex2f(x + w*0.4f, y + h*0.25f);
            glVertex2f(x + w*0.4f, y + h*0.65f); glVertex2f(x + w*0.6f, y + h*0.65f);
            glVertex2f(x + w*0.6f, y + h*0.65f); glVertex2f(x + w*0.6f, y + h*0.75f);
            glVertex2f(x + w*0.6f, y + h*0.75f); glVertex2f(x + w*0.4f, y + h*0.75f);
            glVertex2f(x + w*0.4f, y + h*0.75f); glVertex2f(x + w*0.4f, y + h*0.65f);
            break;
        case '-':
            glVertex2f(x + w*0.2f, y + h*0.5f); glVertex2f(x + w*0.8f, y + h*0.5f);
            break;
        case '+':
            glVertex2f(x + w*0.2f, y + h*0.5f); glVertex2f(x + w*0.8f, y + h*0.5f);
            glVertex2f(x + w*0.5f, y + h*0.25f); glVertex2f(x + w*0.5f, y + h*0.75f);
            break;
        case '/':
            glVertex2f(x, y + h); glVertex2f(x + w, y);
            break;
        case '%':
            glVertex2f(x, y + h); glVertex2f(x + w, y);
            glVertex2f(x + w*0.2f, y + h*0.1f); glVertex2f(x + w*0.3f, y + h*0.1f);
            glVertex2f(x + w*0.3f, y + h*0.1f); glVertex2f(x + w*0.3f, y + h*0.25f);
            glVertex2f(x + w*0.3f, y + h*0.25f); glVertex2f(x + w*0.2f, y + h*0.25f);
            glVertex2f(x + w*0.2f, y + h*0.25f); glVertex2f(x + w*0.2f, y + h*0.1f);
            glVertex2f(x + w*0.7f, y + h*0.75f); glVertex2f(x + w*0.8f, y + h*0.75f);
            glVertex2f(x + w*0.8f, y + h*0.75f); glVertex2f(x + w*0.8f, y + h*0.9f);
            glVertex2f(x + w*0.8f, y + h*0.9f); glVertex2f(x + w*0.7f, y + h*0.9f);
            glVertex2f(x + w*0.7f, y + h*0.9f); glVertex2f(x + w*0.7f, y + h*0.75f);
            break;
        case ' ':
            // Space - no lines
            break;
        default:
            // Unknown character - draw a box
            glVertex2f(x, y); glVertex2f(x + w, y);
            glVertex2f(x + w, y); glVertex2f(x + w, y + h);
            glVertex2f(x + w, y + h); glVertex2f(x, y + h);
            glVertex2f(x, y + h); glVertex2f(x, y);
            break;
    }
    
    glEnd();
}

void Renderer::renderRect(float x, float y, float width, float height, const Color& color, bool filled) {
    // Switch to 2D rendering
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, screenWidth, screenHeight, 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    glColor4f(color.r, color.g, color.b, color.a);
    
    if (filled) {
        glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
        glEnd();
    } else {
        glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
        glEnd();
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void Renderer::renderCircle(float x, float y, float radius, const Color& color, bool filled) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, screenWidth, screenHeight, 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    glColor4f(color.r, color.g, color.b, color.a);
    
    int segments = 32;
    if (filled) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for (int i = 0; i <= segments; i++) {
            float angle = 2.0f * PI * i / segments;
            glVertex2f(x + radius * std::cos(angle), y + radius * std::sin(angle));
        }
        glEnd();
    } else {
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < segments; i++) {
            float angle = 2.0f * PI * i / segments;
            glVertex2f(x + radius * std::cos(angle), y + radius * std::sin(angle));
        }
        glEnd();
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void Renderer::renderProgressBar(float x, float y, float width, float height, float progress,
                                 const Color& bgColor, const Color& fillColor) {
    // Background
    renderRect(x, y, width, height, bgColor, true);
    
    // Fill
    float fillWidth = width * progress;
    if (fillWidth > 0) {
        renderRect(x, y, fillWidth, height, fillColor, true);
    }
    
    // Border
    renderRect(x, y, width, height, Color::White(), false);
}

void Renderer::renderCube(const Vector3& position, const Vector3& size, const Color& color) {
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glScalef(size.x, size.y, size.z);
    
    glColor4f(color.r, color.g, color.b, color.a);
    
    glBegin(GL_QUADS);
    // Front face
    glNormal3f(0, 0, 1);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    
    // Back face
    glNormal3f(0, 0, -1);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    
    // Top face
    glNormal3f(0, 1, 0);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    
    // Bottom face
    glNormal3f(0, -1, 0);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    
    // Right face
    glNormal3f(1, 0, 0);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    
    // Left face
    glNormal3f(-1, 0, 0);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glEnd();
    
    glPopMatrix();
}

void Renderer::renderPlane3D(const Vector3& position, const Vector3& rotation,
                             float wingspan, float length, const Color& primaryColor, const Color& secondaryColor,
                             float gearState, float flapsState) {
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glRotatef(rotation.y, 0, 1, 0);  // Yaw
    glRotatef(rotation.x, 1, 0, 0);  // Pitch
    glRotatef(rotation.z, 0, 0, 1);  // Roll
    
    // Fuselage (main body)
    glColor4f(primaryColor.r, primaryColor.g, primaryColor.b, primaryColor.a);
    
    float fuselageWidth = length * 0.08f;
    float fuselageHeight = length * 0.1f;
    
    glBegin(GL_QUADS);
    // Top
    glNormal3f(0, 1, 0);
    glVertex3f(-fuselageWidth, fuselageHeight, -length * 0.5f);
    glVertex3f(fuselageWidth, fuselageHeight, -length * 0.5f);
    glVertex3f(fuselageWidth, fuselageHeight, length * 0.5f);
    glVertex3f(-fuselageWidth, fuselageHeight, length * 0.5f);
    
    // Bottom
    glNormal3f(0, -1, 0);
    glVertex3f(-fuselageWidth, -fuselageHeight, -length * 0.5f);
    glVertex3f(-fuselageWidth, -fuselageHeight, length * 0.5f);
    glVertex3f(fuselageWidth, -fuselageHeight, length * 0.5f);
    glVertex3f(fuselageWidth, -fuselageHeight, -length * 0.5f);
    
    // Left side
    glNormal3f(-1, 0, 0);
    glVertex3f(-fuselageWidth, -fuselageHeight, -length * 0.5f);
    glVertex3f(-fuselageWidth, fuselageHeight, -length * 0.5f);
    glVertex3f(-fuselageWidth, fuselageHeight, length * 0.5f);
    glVertex3f(-fuselageWidth, -fuselageHeight, length * 0.5f);
    
    // Right side
    glNormal3f(1, 0, 0);
    glVertex3f(fuselageWidth, -fuselageHeight, -length * 0.5f);
    glVertex3f(fuselageWidth, -fuselageHeight, length * 0.5f);
    glVertex3f(fuselageWidth, fuselageHeight, length * 0.5f);
    glVertex3f(fuselageWidth, fuselageHeight, -length * 0.5f);
    glEnd();
    
    // Nose cone
    glBegin(GL_TRIANGLES);
    glNormal3f(0, 0, -1);
    glVertex3f(0, 0, -length * 0.5f - length * 0.15f);
    glVertex3f(-fuselageWidth, fuselageHeight, -length * 0.5f);
    glVertex3f(fuselageWidth, fuselageHeight, -length * 0.5f);
    
    glVertex3f(0, 0, -length * 0.5f - length * 0.15f);
    glVertex3f(fuselageWidth, -fuselageHeight, -length * 0.5f);
    glVertex3f(-fuselageWidth, -fuselageHeight, -length * 0.5f);
    
    glVertex3f(0, 0, -length * 0.5f - length * 0.15f);
    glVertex3f(-fuselageWidth, -fuselageHeight, -length * 0.5f);
    glVertex3f(-fuselageWidth, fuselageHeight, -length * 0.5f);
    
    glVertex3f(0, 0, -length * 0.5f - length * 0.15f);
    glVertex3f(fuselageWidth, fuselageHeight, -length * 0.5f);
    glVertex3f(fuselageWidth, -fuselageHeight, -length * 0.5f);
    glEnd();
    
    // Wings
    glColor4f(secondaryColor.r, secondaryColor.g, secondaryColor.b, secondaryColor.a);
    
    float wingThickness = 0.3f;
    float wingChord = length * 0.25f;  // Width of wing
    
    glBegin(GL_QUADS);
    // Left wing - top
    glNormal3f(0, 1, 0);
    glVertex3f(-fuselageWidth, 0, -wingChord * 0.3f);
    glVertex3f(-wingspan * 0.5f, 0, 0);
    glVertex3f(-wingspan * 0.5f, 0, wingChord * 0.7f);
    glVertex3f(-fuselageWidth, 0, wingChord * 0.7f);
    
    // Left wing - bottom
    glNormal3f(0, -1, 0);
    glVertex3f(-fuselageWidth, -wingThickness, -wingChord * 0.3f);
    glVertex3f(-fuselageWidth, -wingThickness, wingChord * 0.7f);
    glVertex3f(-wingspan * 0.5f, -wingThickness * 0.3f, wingChord * 0.7f);
    glVertex3f(-wingspan * 0.5f, -wingThickness * 0.3f, 0);
    
    // Right wing - top
    glNormal3f(0, 1, 0);
    glVertex3f(fuselageWidth, 0, -wingChord * 0.3f);
    glVertex3f(fuselageWidth, 0, wingChord * 0.7f);
    glVertex3f(wingspan * 0.5f, 0, wingChord * 0.7f);
    glVertex3f(wingspan * 0.5f, 0, 0);
    
    // Right wing - bottom
    glNormal3f(0, -1, 0);
    glVertex3f(fuselageWidth, -wingThickness, -wingChord * 0.3f);
    glVertex3f(wingspan * 0.5f, -wingThickness * 0.3f, 0);
    glVertex3f(wingspan * 0.5f, -wingThickness * 0.3f, wingChord * 0.7f);
    glVertex3f(fuselageWidth, -wingThickness, wingChord * 0.7f);
    glEnd();
    
    // Tail (vertical stabilizer)
    glColor4f(primaryColor.r, primaryColor.g, primaryColor.b, primaryColor.a);
    
    float tailHeight = length * 0.2f;
    float tailWidth = 0.2f;
    
    glBegin(GL_TRIANGLES);
    glNormal3f(1, 0, 0);
    glVertex3f(tailWidth, fuselageHeight, length * 0.3f);
    glVertex3f(tailWidth, fuselageHeight + tailHeight, length * 0.45f);
    glVertex3f(tailWidth, fuselageHeight, length * 0.5f);
    
    glNormal3f(-1, 0, 0);
    glVertex3f(-tailWidth, fuselageHeight, length * 0.3f);
    glVertex3f(-tailWidth, fuselageHeight, length * 0.5f);
    glVertex3f(-tailWidth, fuselageHeight + tailHeight, length * 0.45f);
    glEnd();
    
    // Horizontal stabilizers
    glBegin(GL_QUADS);
    float stabSpan = wingspan * 0.35f;
    float stabChord = length * 0.12f;
    
    // Left stabilizer
    glNormal3f(0, 1, 0);
    glVertex3f(-fuselageWidth, fuselageHeight * 0.8f, length * 0.35f);
    glVertex3f(-stabSpan, fuselageHeight * 0.8f, length * 0.4f);
    glVertex3f(-stabSpan, fuselageHeight * 0.8f, length * 0.4f + stabChord);
    glVertex3f(-fuselageWidth, fuselageHeight * 0.8f, length * 0.35f + stabChord);
    
    // Right stabilizer
    glNormal3f(0, 1, 0);
    glVertex3f(fuselageWidth, fuselageHeight * 0.8f, length * 0.35f);
    glVertex3f(fuselageWidth, fuselageHeight * 0.8f, length * 0.35f + stabChord);
    glVertex3f(stabSpan, fuselageHeight * 0.8f, length * 0.4f + stabChord);
    glVertex3f(stabSpan, fuselageHeight * 0.8f, length * 0.4f);
    glEnd();
    
    // ===== LANDING GEAR (Animated) =====
    if (gearState > 0.01f) {  // Only draw if gear is at least slightly extended
        glColor4f(0.2f, 0.2f, 0.2f, 1.0f);  // Dark grey
        
        float gearLength = length * 0.12f * gearState;  // Animated length
        float gearWidth = length * 0.015f;
        float wheelRadius = length * 0.03f * gearState;
        
        // Front gear (nose gear)
        float noseGearZ = -length * 0.35f;
        float gearAngle = (1.0f - gearState) * 90.0f;  // Rotates up when retracting
        
        glPushMatrix();
        glTranslatef(0, -fuselageHeight, noseGearZ);
        glRotatef(gearAngle, 1, 0, 0);  // Rotate to fold up
        
        // Strut
        glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glVertex3f(-gearWidth, 0, 0);
        glVertex3f(gearWidth, 0, 0);
        glVertex3f(gearWidth, -gearLength, 0);
        glVertex3f(-gearWidth, -gearLength, 0);
        glEnd();
        
        // Wheel (simple box for now)
        glColor4f(0.1f, 0.1f, 0.1f, 1.0f);  // Black wheel
        glTranslatef(0, -gearLength, 0);
        glBegin(GL_QUADS);
        // Front
        glVertex3f(-wheelRadius, -wheelRadius * 0.5f, wheelRadius * 0.3f);
        glVertex3f(wheelRadius, -wheelRadius * 0.5f, wheelRadius * 0.3f);
        glVertex3f(wheelRadius, wheelRadius * 0.5f, wheelRadius * 0.3f);
        glVertex3f(-wheelRadius, wheelRadius * 0.5f, wheelRadius * 0.3f);
        // Back
        glVertex3f(-wheelRadius, -wheelRadius * 0.5f, -wheelRadius * 0.3f);
        glVertex3f(-wheelRadius, wheelRadius * 0.5f, -wheelRadius * 0.3f);
        glVertex3f(wheelRadius, wheelRadius * 0.5f, -wheelRadius * 0.3f);
        glVertex3f(wheelRadius, -wheelRadius * 0.5f, -wheelRadius * 0.3f);
        glEnd();
        glPopMatrix();
        
        // Left main gear
        float mainGearX = wingspan * 0.15f;
        float mainGearZ = length * 0.05f;
        
        glColor4f(0.2f, 0.2f, 0.2f, 1.0f);
        glPushMatrix();
        glTranslatef(-mainGearX, -fuselageHeight, mainGearZ);
        glRotatef(gearAngle, 1, 0, 0);
        
        glBegin(GL_QUADS);
        glNormal3f(1, 0, 0);
        glVertex3f(0, 0, -gearWidth);
        glVertex3f(0, 0, gearWidth);
        glVertex3f(0, -gearLength * 1.2f, gearWidth);
        glVertex3f(0, -gearLength * 1.2f, -gearWidth);
        glEnd();
        
        // Left wheel
        glColor4f(0.1f, 0.1f, 0.1f, 1.0f);
        glTranslatef(0, -gearLength * 1.2f, 0);
        glBegin(GL_QUADS);
        glVertex3f(-wheelRadius * 0.5f, -wheelRadius, wheelRadius);
        glVertex3f(wheelRadius * 0.5f, -wheelRadius, wheelRadius);
        glVertex3f(wheelRadius * 0.5f, wheelRadius, wheelRadius);
        glVertex3f(-wheelRadius * 0.5f, wheelRadius, wheelRadius);
        glVertex3f(-wheelRadius * 0.5f, -wheelRadius, -wheelRadius);
        glVertex3f(-wheelRadius * 0.5f, wheelRadius, -wheelRadius);
        glVertex3f(wheelRadius * 0.5f, wheelRadius, -wheelRadius);
        glVertex3f(wheelRadius * 0.5f, -wheelRadius, -wheelRadius);
        glEnd();
        glPopMatrix();
        
        // Right main gear
        glColor4f(0.2f, 0.2f, 0.2f, 1.0f);
        glPushMatrix();
        glTranslatef(mainGearX, -fuselageHeight, mainGearZ);
        glRotatef(gearAngle, 1, 0, 0);
        
        glBegin(GL_QUADS);
        glNormal3f(-1, 0, 0);
        glVertex3f(0, 0, gearWidth);
        glVertex3f(0, 0, -gearWidth);
        glVertex3f(0, -gearLength * 1.2f, -gearWidth);
        glVertex3f(0, -gearLength * 1.2f, gearWidth);
        glEnd();
        
        // Right wheel
        glColor4f(0.1f, 0.1f, 0.1f, 1.0f);
        glTranslatef(0, -gearLength * 1.2f, 0);
        glBegin(GL_QUADS);
        glVertex3f(-wheelRadius * 0.5f, -wheelRadius, wheelRadius);
        glVertex3f(wheelRadius * 0.5f, -wheelRadius, wheelRadius);
        glVertex3f(wheelRadius * 0.5f, wheelRadius, wheelRadius);
        glVertex3f(-wheelRadius * 0.5f, wheelRadius, wheelRadius);
        glVertex3f(-wheelRadius * 0.5f, -wheelRadius, -wheelRadius);
        glVertex3f(-wheelRadius * 0.5f, wheelRadius, -wheelRadius);
        glVertex3f(wheelRadius * 0.5f, wheelRadius, -wheelRadius);
        glVertex3f(wheelRadius * 0.5f, -wheelRadius, -wheelRadius);
        glEnd();
        glPopMatrix();
    }
    
    // ===== FLAPS (Animated) =====
    if (flapsState > 0.01f) {
        glColor4f(secondaryColor.r * 0.8f, secondaryColor.g * 0.8f, secondaryColor.b * 0.8f, 1.0f);
        
        float flapAngle = flapsState * 30.0f;  // Max 30 degree deflection
        float flapWidth = wingspan * 0.15f;
        float flapChord = length * 0.08f;
        float flapThickness = 0.15f;
        
        // Left wing flap
        glPushMatrix();
        glTranslatef(-wingspan * 0.25f, -0.2f, length * 0.08f);
        glRotatef(flapAngle, 1, 0, 0);  // Rotate down
        
        glBegin(GL_QUADS);
        // Top
        glNormal3f(0, 1, 0);
        glVertex3f(-flapWidth * 0.5f, 0, 0);
        glVertex3f(flapWidth * 0.5f, 0, 0);
        glVertex3f(flapWidth * 0.5f, 0, flapChord);
        glVertex3f(-flapWidth * 0.5f, 0, flapChord);
        // Bottom
        glNormal3f(0, -1, 0);
        glVertex3f(-flapWidth * 0.5f, -flapThickness, 0);
        glVertex3f(-flapWidth * 0.5f, -flapThickness, flapChord);
        glVertex3f(flapWidth * 0.5f, -flapThickness, flapChord);
        glVertex3f(flapWidth * 0.5f, -flapThickness, 0);
        glEnd();
        glPopMatrix();
        
        // Right wing flap
        glPushMatrix();
        glTranslatef(wingspan * 0.25f, -0.2f, length * 0.08f);
        glRotatef(flapAngle, 1, 0, 0);
        
        glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glVertex3f(-flapWidth * 0.5f, 0, 0);
        glVertex3f(flapWidth * 0.5f, 0, 0);
        glVertex3f(flapWidth * 0.5f, 0, flapChord);
        glVertex3f(-flapWidth * 0.5f, 0, flapChord);
        glNormal3f(0, -1, 0);
        glVertex3f(-flapWidth * 0.5f, -flapThickness, 0);
        glVertex3f(-flapWidth * 0.5f, -flapThickness, flapChord);
        glVertex3f(flapWidth * 0.5f, -flapThickness, flapChord);
        glVertex3f(flapWidth * 0.5f, -flapThickness, 0);
        glEnd();
        glPopMatrix();
    }
    
    glPopMatrix();
}

void Renderer::renderAircraft(const Aircraft* aircraft, const Camera* camera) {
    if (!aircraft) return;
    
    const AircraftSpecs& specs = aircraft->getSpecs();
    
    renderPlane3D(
        aircraft->getPosition(),
        aircraft->getRotation(),
        specs.wingSpan,
        specs.length,
        specs.primaryColor,
        specs.secondaryColor,
        aircraft->getGearAnimationState(),
        aircraft->getFlapsAnimationState()
    );
}

void Renderer::renderTerrain(const Terrain* terrain, const Camera* camera) {
    if (!terrain) return;
    
    // Render terrain chunks
    const auto& chunks = terrain->getChunks();
    
    if (chunks.empty()) {
        // Fallback: render simple ground plane
        glColor4f(0.2f, 0.5f, 0.2f, 1.0f);
        glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glVertex3f(-5000, 0, -5000);
        glVertex3f(5000, 0, -5000);
        glVertex3f(5000, 0, 5000);
        glVertex3f(-5000, 0, 5000);
        glEnd();
    } else {
        // Render each chunk's terrain mesh
        glBegin(GL_TRIANGLES);
        for (const auto& [coord, chunk] : chunks) {
            if (!chunk || !chunk->generated) continue;
            
            int chunkSize = terrain->getChunkSize();
            const auto& vertices = chunk->vertices;
            const auto& normals = chunk->normals;
            const auto& colors = chunk->colors;
            
            // Render chunk as grid of triangles
            for (int z = 0; z < chunkSize - 1; z++) {
                for (int x = 0; x < chunkSize - 1; x++) {
                    int idx = z * chunkSize + x;
                    
                    // First triangle
                    if (idx < (int)vertices.size() && idx < (int)colors.size()) {
                        glColor4f(colors[idx].r, colors[idx].g, colors[idx].b, colors[idx].a);
                        glNormal3f(normals[idx].x, normals[idx].y, normals[idx].z);
                        glVertex3f(vertices[idx].x, vertices[idx].y, vertices[idx].z);
                    }
                    
                    int idx2 = (z + 1) * chunkSize + x;
                    if (idx2 < (int)vertices.size() && idx2 < (int)colors.size()) {
                        glColor4f(colors[idx2].r, colors[idx2].g, colors[idx2].b, colors[idx2].a);
                        glNormal3f(normals[idx2].x, normals[idx2].y, normals[idx2].z);
                        glVertex3f(vertices[idx2].x, vertices[idx2].y, vertices[idx2].z);
                    }
                    
                    int idx3 = z * chunkSize + (x + 1);
                    if (idx3 < (int)vertices.size() && idx3 < (int)colors.size()) {
                        glColor4f(colors[idx3].r, colors[idx3].g, colors[idx3].b, colors[idx3].a);
                        glNormal3f(normals[idx3].x, normals[idx3].y, normals[idx3].z);
                        glVertex3f(vertices[idx3].x, vertices[idx3].y, vertices[idx3].z);
                    }
                    
                    // Second triangle
                    if (idx3 < (int)vertices.size() && idx3 < (int)colors.size()) {
                        glColor4f(colors[idx3].r, colors[idx3].g, colors[idx3].b, colors[idx3].a);
                        glNormal3f(normals[idx3].x, normals[idx3].y, normals[idx3].z);
                        glVertex3f(vertices[idx3].x, vertices[idx3].y, vertices[idx3].z);
                    }
                    
                    if (idx2 < (int)vertices.size() && idx2 < (int)colors.size()) {
                        glColor4f(colors[idx2].r, colors[idx2].g, colors[idx2].b, colors[idx2].a);
                        glNormal3f(normals[idx2].x, normals[idx2].y, normals[idx2].z);
                        glVertex3f(vertices[idx2].x, vertices[idx2].y, vertices[idx2].z);
                    }
                    
                    int idx4 = (z + 1) * chunkSize + (x + 1);
                    if (idx4 < (int)vertices.size() && idx4 < (int)colors.size()) {
                        glColor4f(colors[idx4].r, colors[idx4].g, colors[idx4].b, colors[idx4].a);
                        glNormal3f(normals[idx4].x, normals[idx4].y, normals[idx4].z);
                        glVertex3f(vertices[idx4].x, vertices[idx4].y, vertices[idx4].z);
                    }
                }
            }
        }
        glEnd();
    }
    
    // Render runway
    const Runway& runway = terrain->getRunway();
    glColor4f(0.3f, 0.3f, 0.35f, 1.0f);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0.01f);
    glVertex3f(runway.startX, runway.height + 0.1f, runway.startZ - runway.width / 2);
    glVertex3f(runway.endX, runway.height + 0.1f, runway.startZ - runway.width / 2);
    glVertex3f(runway.endX, runway.height + 0.1f, runway.startZ + runway.width / 2);
    glVertex3f(runway.startX, runway.height + 0.1f, runway.startZ + runway.width / 2);
    glEnd();
    
    // Runway markings
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    for (float z = runway.startX; z < runway.endX; z += 50) {
        glBegin(GL_QUADS);
        glNormal3f(0, 1, 0.02f);
        glVertex3f(z, runway.height + 0.2f, runway.startZ - 2);
        glVertex3f(z + 25, runway.height + 0.2f, runway.startZ - 2);
        glVertex3f(z + 25, runway.height + 0.2f, runway.startZ + 2);
        glVertex3f(z, runway.height + 0.2f, runway.startZ + 2);
        glEnd();
    }
}

void Renderer::renderSky(const Sky* sky, const Camera* camera) {
    if (!sky) return;
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    
    // Render sky gradient
    Color top = sky->getSkyColorTop();
    Color horizon = sky->getSkyColorHorizon();
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glBegin(GL_QUADS);
    glColor4f(top.r, top.g, top.b, 1.0f);
    glVertex2f(-1, 1);
    glVertex2f(1, 1);
    glColor4f(horizon.r, horizon.g, horizon.b, 1.0f);
    glVertex2f(1, -0.3f);
    glVertex2f(-1, -0.3f);
    
    glColor4f(horizon.r, horizon.g, horizon.b, 1.0f);
    glVertex2f(-1, -0.3f);
    glVertex2f(1, -0.3f);
    glVertex2f(1, -1);
    glVertex2f(-1, -1);
    glEnd();
    
    // Render sun
    Vector3 sunDir = sky->getSunDirection();
    if (sunDir.y > 0) {
        Color sunColor = sky->getSunColor();
        float sunX = sunDir.x * 0.8f;
        float sunY = sunDir.y * 0.8f;
        float sunSize = 0.08f;
        
        glColor4f(sunColor.r, sunColor.g, sunColor.b, 1.0f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(sunX, sunY);
        for (int i = 0; i <= 20; i++) {
            float angle = 2.0f * PI * i / 20;
            glVertex2f(sunX + sunSize * std::cos(angle), sunY + sunSize * std::sin(angle));
        }
        glEnd();
    }
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void Renderer::renderHUD(const Aircraft* aircraft) {
    if (!aircraft) return;
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    
    float margin = 20.0f;
    float panelWidth = 250.0f;
    float panelHeight = 180.0f;
    
    // Left panel - Flight data
    renderRect(margin, screenHeight - panelHeight - margin, panelWidth, panelHeight, 
               Color(0.0f, 0.0f, 0.0f, 0.6f), true);
    renderRect(margin, screenHeight - panelHeight - margin, panelWidth, panelHeight, 
               Color(0.3f, 0.8f, 0.3f, 0.8f), false);
    
    float textX = margin + 15.0f;
    float textY = screenHeight - panelHeight - margin + 20.0f;
    float lineHeight = 22.0f;
    Color textColor = Color(0.3f, 1.0f, 0.3f, 1.0f);
    
    char buffer[64];
    
    snprintf(buffer, sizeof(buffer), "SPEED: %.0f KTS", aircraft->getSpeed() * 1.944f);
    renderText(buffer, textX, textY, 1.0f, textColor);
    textY += lineHeight;
    
    snprintf(buffer, sizeof(buffer), "ALT: %.0f FT", aircraft->getAltitude() * 3.281f);
    renderText(buffer, textX, textY, 1.0f, textColor);
    textY += lineHeight;
    
    snprintf(buffer, sizeof(buffer), "HDG: %.0f", aircraft->getHeading());
    renderText(buffer, textX, textY, 1.0f, textColor);
    textY += lineHeight;
    
    snprintf(buffer, sizeof(buffer), "VS: %.0f FPM", aircraft->getVerticalSpeed() * 196.85f);
    renderText(buffer, textX, textY, 1.0f, textColor);
    textY += lineHeight;
    
    snprintf(buffer, sizeof(buffer), "THROTTLE: %.0f%%", aircraft->getThrottle() * 100.0f);
    renderText(buffer, textX, textY, 1.0f, textColor);
    textY += lineHeight;
    
    snprintf(buffer, sizeof(buffer), "FLAPS: %d", aircraft->getFlapsLevel());
    renderText(buffer, textX, textY, 1.0f, textColor);
    
    // Right panel - Aircraft status
    float rightPanelX = screenWidth - panelWidth - margin;
    renderRect(rightPanelX, screenHeight - panelHeight - margin, panelWidth, panelHeight,
               Color(0.0f, 0.0f, 0.0f, 0.6f), true);
    renderRect(rightPanelX, screenHeight - panelHeight - margin, panelWidth, panelHeight,
               Color(0.3f, 0.8f, 0.3f, 0.8f), false);
    
    textX = rightPanelX + 15.0f;
    textY = screenHeight - panelHeight - margin + 20.0f;
    
    renderText(aircraft->getSpecs().name.c_str(), textX, textY, 1.0f, textColor);
    textY += lineHeight;
    
    snprintf(buffer, sizeof(buffer), "GEAR: %s", aircraft->isLandingGearDown() ? "DOWN" : "UP");
    renderText(buffer, textX, textY, 1.0f, 
               aircraft->isLandingGearDown() ? Color(0.3f, 1.0f, 0.3f) : Color(1.0f, 0.8f, 0.3f));
    textY += lineHeight;
    
    snprintf(buffer, sizeof(buffer), "GROUND: %s", aircraft->isOnGround() ? "YES" : "NO");
    renderText(buffer, textX, textY, 1.0f, textColor);
    textY += lineHeight;
    
    // Stall warning
    if (aircraft->isStalling()) {
        renderText("STALL WARNING!", textX, textY, 1.2f, Color::Red());
    }
    
    // Center HUD - Artificial horizon reference
    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f;
    
    // Crosshairs
    Color crosshairColor = Color(0.3f, 1.0f, 0.3f, 0.8f);
    renderRect(centerX - 30, centerY - 1, 20, 2, crosshairColor, true);
    renderRect(centerX + 10, centerY - 1, 20, 2, crosshairColor, true);
    renderRect(centerX - 1, centerY - 30, 2, 20, crosshairColor, true);
    renderRect(centerX - 1, centerY + 10, 2, 20, crosshairColor, true);
    
    // Pitch ladder
    float pitch = aircraft->getPitch();
    for (int deg = -30; deg <= 30; deg += 10) {
        if (deg == 0) continue;
        float y = centerY + (pitch - deg) * 3.0f;
        if (y > centerY - 100 && y < centerY + 100) {
            float lineWidth = (deg % 20 == 0) ? 40.0f : 20.0f;
            renderRect(centerX - lineWidth, y, lineWidth * 2, 1, crosshairColor, true);
            
            char degStr[8];
            snprintf(degStr, sizeof(degStr), "%d", deg);
            renderText(degStr, centerX - lineWidth - 25, y - 5, 0.7f, crosshairColor);
        }
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void Renderer::renderMinimap(const Aircraft* aircraft, const Terrain* terrain) {
    if (!aircraft) return;
    
    float mapSize = 150.0f;
    float mapX = screenWidth - mapSize - 20.0f;
    float mapY = 20.0f;
    
    // Background
    renderRect(mapX, mapY, mapSize, mapSize, Color(0.0f, 0.0f, 0.0f, 0.7f), true);
    renderRect(mapX, mapY, mapSize, mapSize, Color(0.3f, 0.8f, 0.3f, 0.8f), false);
    
    // Aircraft position indicator (center)
    float centerX = mapX + mapSize / 2.0f;
    float centerY = mapY + mapSize / 2.0f;
    
    // Aircraft triangle
    float heading = aircraft->getHeading() * DEG_TO_RAD;
    float triSize = 8.0f;
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, screenWidth, screenHeight, 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(centerX + triSize * std::sin(heading), centerY - triSize * std::cos(heading));
    glVertex2f(centerX + triSize * 0.5f * std::sin(heading + 2.5f), centerY - triSize * 0.5f * std::cos(heading + 2.5f));
    glVertex2f(centerX + triSize * 0.5f * std::sin(heading - 2.5f), centerY - triSize * 0.5f * std::cos(heading - 2.5f));
    glEnd();
    
    // Runway indicator
    glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
    float runwayX = centerX;
    float runwayY = centerY - aircraft->getPosition().z / 50.0f;
    runwayY = std::max(mapY + 10.0f, std::min(mapY + mapSize - 10.0f, runwayY));
    glBegin(GL_LINES);
    glVertex2f(runwayX - 5, runwayY);
    glVertex2f(runwayX + 5, runwayY);
    glEnd();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    
    // North indicator
    renderText("N", mapX + mapSize / 2 - 5, mapY + 5, 0.8f, Color::White());
}

void Renderer::renderSphere(const Vector3& position, float radius, const Color& color) {
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    
    glColor4f(color.r, color.g, color.b, color.a);
    
    int stacks = 16;
    int slices = 16;
    
    for (int i = 0; i < stacks; i++) {
        float lat0 = PI * (-0.5f + (float)i / stacks);
        float lat1 = PI * (-0.5f + (float)(i + 1) / stacks);
        float y0 = std::sin(lat0) * radius;
        float y1 = std::sin(lat1) * radius;
        float r0 = std::cos(lat0) * radius;
        float r1 = std::cos(lat1) * radius;
        
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; j++) {
            float lng = 2 * PI * (float)j / slices;
            float x = std::cos(lng);
            float z = std::sin(lng);
            
            glNormal3f(x * r0 / radius, y0 / radius, z * r0 / radius);
            glVertex3f(x * r0, y0, z * r0);
            
            glNormal3f(x * r1 / radius, y1 / radius, z * r1 / radius);
            glVertex3f(x * r1, y1, z * r1);
        }
        glEnd();
    }
    
    glPopMatrix();
}

void Renderer::renderCylinder(const Vector3& position, float radius, float height, const Color& color) {
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    
    glColor4f(color.r, color.g, color.b, color.a);
    
    int slices = 20;
    
    // Side
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; i++) {
        float angle = 2 * PI * i / slices;
        float x = std::cos(angle) * radius;
        float z = std::sin(angle) * radius;
        
        glNormal3f(std::cos(angle), 0, std::sin(angle));
        glVertex3f(x, 0, z);
        glVertex3f(x, height, z);
    }
    glEnd();
    
    // Top cap
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0);
    glVertex3f(0, height, 0);
    for (int i = 0; i <= slices; i++) {
        float angle = 2 * PI * i / slices;
        glVertex3f(std::cos(angle) * radius, height, std::sin(angle) * radius);
    }
    glEnd();
    
    // Bottom cap
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, -1, 0);
    glVertex3f(0, 0, 0);
    for (int i = slices; i >= 0; i--) {
        float angle = 2 * PI * i / slices;
        glVertex3f(std::cos(angle) * radius, 0, std::sin(angle) * radius);
    }
    glEnd();
    
    glPopMatrix();
}
