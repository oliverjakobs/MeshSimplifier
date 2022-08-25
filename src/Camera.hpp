#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Most of this code ist taken from:
// https://asliceofrendering.com/camera/2019/11/30/ArcballCamera/

class Camera
{
private:
    glm::mat4 viewMat;
    glm::vec3 eye;      // camera position in 3D
    glm::vec3 lookAt;   // point that the camera is looking at
    glm::vec3 up;       // orientation of the camera

    glm::vec2 screen;   // size of the screen

    bool dragging = false;

    void updateViewMatrix();
public:
    Camera() = default;
    Camera(glm::vec3 eye, glm::vec2 screen, glm::vec3 lookat = glm::vec3(0,0,0), glm::vec3 up = glm::vec3(0,1,0));

    void setView(glm::vec3 eye, glm::vec3 lookat, glm::vec3 up);
    void setScreenSize(glm::vec2 size);

    void updateMouse(float x, float y, float lastX, float lastY);

    void startDragging() { dragging = true; }
    void stopDragging()  { dragging = false; }

    glm::vec3 getPosition() const { return eye; }
    glm::mat4 getViewMatrix() const { return viewMat; }
    glm::vec2 getScreenSize() const { return screen; }
    float getAspectRatio() const { return screen.y != 0.0f ? screen.x / screen.y : screen.x; }

    float getWidth() const { return screen.x; }
    float getHeight() const { return screen.y; }

    // camera forward is -z
    glm::vec3 getViewDir() const { return -glm::transpose(viewMat)[2]; }
    glm::vec3 getRightVector() const { return glm::transpose(viewMat)[0]; }
};
