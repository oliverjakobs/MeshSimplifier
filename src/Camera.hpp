#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{

private:
    glm::mat4 viewMat;
    glm::vec3 eye;      // Camera position in 3D
    glm::vec3 lookAt;   // Point that the camera is looking at
    glm::vec3 up;       // Orientation of the camera

    glm::vec2 screenSize;
    bool dragging = false;

    void updateViewMatrix();
public:
    Camera() = default;
    Camera(glm::vec3 eye, glm::vec3 lookat, glm::vec3 up);

    void setView(glm::vec3 eye, glm::vec3 lookat, glm::vec3 up);
    void setScreenSize(float width, float height);

    void updateMouse(float x, float y, float lastX, float lastY);

    void startDragging() { dragging = true; }
    void stopDragging()  { dragging = false; }

    glm::vec3 getPosition() const { return eye; }
    glm::mat4 getViewMatrix() const { return viewMat; }
    glm::vec2 getScreenSize() const { return screenSize; }
    float getAspectRatio() const { return screenSize.x / screenSize.y; }

    float getWidth() const { return screenSize.x; }
    float getHeight() const { return screenSize.y; }

    // Camera forward is -z
    glm::vec3 getViewDir() const { return -glm::transpose(viewMat)[2]; }
    glm::vec3 getRightVector() const { return glm::transpose(viewMat)[0]; }
};
