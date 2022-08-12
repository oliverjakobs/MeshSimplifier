#pragma once

#include <Ignis/Ignis.h>
#include <GLFW/glfw3.h>

class GLFWApplication
{
protected:
    GLFWwindow* window;

	int width, height;

    // timing
    float deltaTime;
    float lastFrame;
public:
    GLFWApplication(const char* title, int width, int height, bool debug);
    ~GLFWApplication();

    void run();

    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;

    float getAspectRatio() const;
};