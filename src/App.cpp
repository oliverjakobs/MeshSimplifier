#include "App.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <stdio.h>

static void glfwErrorCallback(int error, const char* desc);
static void ignisErrorCallback(ignisErrorLevel level, const char* desc);

static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mod);
static void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos);

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

    // set glfw callbacks
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mouseMoveCallback);

    ignisSetErrorCallback(ignisErrorCallback);
    if (!ignisInit(debug))
        return;

    // configure global opengl state
    ignisSetClearColor({ 0.5f, 0.5f, 1.0f, 1.0f });
    glEnable(GL_DEPTH_TEST);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

GLFWApplication::~GLFWApplication()
{
    // shutdown ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // shutdown glfw
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

        onUpdate(deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        onRender();

        // start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        onRenderGui();

        // render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = (GLFWApplication*)glfwGetWindowUserPointer(window);
    app->onResize(width, height);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mod)
{
    auto app = (GLFWApplication*)glfwGetWindowUserPointer(window);
    app->onMouseButton(button, action);
}

void mouseMoveCallback(GLFWwindow* window, double xPos, double yPos)
{
    auto app = (GLFWApplication*)glfwGetWindowUserPointer(window);
    app->onMouseMove((float)xPos, (float)yPos);
}

void glfwErrorCallback(int error, const char* desc)
{
    printf("[Glfw] %d: %s\n", error, desc);
}

void ignisErrorCallback(ignisErrorLevel level, const char* desc)
{
    printf("%s\n", desc);
}