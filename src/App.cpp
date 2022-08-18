#include "App.hpp"

#include <stdio.h>

static void glfwErrorCallback(int error, const char* desc);
static void ignisErrorCallback(ignisErrorLevel level, const char* desc);

GLFWApplication::GLFWApplication(const char* title, int width, int height, bool debug)
    : deltaTime(0.0f), lastFrame(0.0f), window(nullptr)
{
    // initialize glfw
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit())
        return;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (debug) glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // create glfw window
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL)
        return;

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSwapInterval(1); // Enable vsync

    ignisSetErrorCallback(ignisErrorCallback);
    if (!ignisInit(debug))
        return;

    // configure global opengl state
    ignisSetClearColor({ 0.5f, 0.5f, 1.0f, 1.0f });
    glEnable(GL_DEPTH_TEST);
}

GLFWApplication::~GLFWApplication()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void GLFWApplication::run()
{
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        update(deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void glfwErrorCallback(int error, const char* desc)
{
    printf("[Glfw] %d: %s\n", error, desc);
}

void ignisErrorCallback(ignisErrorLevel level, const char* desc)
{
    printf("%s\n", desc);
}