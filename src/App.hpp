#pragma once

#include <Ignis/Ignis.h>
#include <imgui.h>
#include <GLFW/glfw3.h>

class GLFWApplication
{
protected:
    GLFWwindow* window;

    // timing
    float deltaTime;
    float lastFrame;
public:
    GLFWApplication(const char* title, int width, int height, bool debug);
    ~GLFWApplication();

    void run();

    virtual void onResize(int width, int height) {}
    virtual void onMouseButton(int button, int action) {}
    virtual void onMouseMove(float xPos, float yPos) {}

    virtual void onUpdate(float deltaTime) = 0;
    virtual void onRender() = 0;
    virtual void onRenderGui() {};
};