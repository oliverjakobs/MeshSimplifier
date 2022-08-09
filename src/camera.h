#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class Camera
{
public:
    Camera() = default;

    Camera(glm::vec3 eye, glm::vec3 lookat, glm::vec3 up)
        : eye(eye), lookAt(lookat), up(up)
    {
        UpdateViewMatrix();
    }

    glm::mat4x4 GetViewMatrix() const { return viewMat; }

    // Camera forward is -z
    glm::vec3 GetViewDir() const { return -glm::transpose(viewMat)[2]; }
    glm::vec3 GetRightVector() const { return glm::transpose(viewMat)[0]; }

    void SetCameraView(glm::vec3 eye, glm::vec3 lookat, glm::vec3 up)
    {
        this->eye = eye;
        this->lookAt = lookat;
        this->up = up;
        UpdateViewMatrix();
    }

    void setScreenSize(float width, float height)
    {
        screenSize.x = width;
        screenSize.y = height;
    }

    void UpdateViewMatrix()
    {
        viewMat = glm::lookAt(eye, lookAt, up);
    }

    void updateMouse(float x, float y, float lastX, float lastY)
    {
        if (!dragging) return;

        // Get the homogenous position of the camera and pivot point
        glm::vec4 position(eye.x, eye.y, eye.z, 1.0);
        glm::vec4 pivot(lookAt.x, lookAt.y, lookAt.z, 1);

        // step 1 : Calculate the amount of rotation given the mouse movement.
        float deltaAngleX = (glm::two_pi<float>() / screenSize.x); // a movement from left to right = 2*PI = 360 deg
        float deltaAngleY = (glm::pi<float>() / screenSize.y);  // a movement from top to bottom = PI = 180 deg
        float xAngle = (lastX - x) * deltaAngleX;
        float yAngle = (lastY - y) * deltaAngleY;

        // Extra step to handle the problem when the camera direction is the same as the up vector
        float cosAngle = glm::dot(GetViewDir(), up);
        if (cosAngle * glm::sign(deltaAngleY) > 0.99f)
            deltaAngleY = 0;

        // step 2: Rotate the camera around the pivot point on the first axis.
        glm::mat4x4 rotationMatrixX(1.0f);
        rotationMatrixX = glm::rotate(rotationMatrixX, xAngle, up);
        position = (rotationMatrixX * (position - pivot)) + pivot;

        // step 3: Rotate the camera around the pivot point on the second axis.
        glm::mat4x4 rotationMatrixY(1.0f);
        rotationMatrixY = glm::rotate(rotationMatrixY, yAngle, GetRightVector());
        eye = (rotationMatrixY * (position - pivot)) + pivot;

        // Update the camera view
        viewMat = glm::lookAt(eye, lookAt, up);
    }

    void startDragging()
    { 
        dragging = true;
    }

    void stopDragging()
    {
        dragging = false;
    }

    glm::vec3 getPosition() const { return eye; }

private:
    glm::mat4x4 viewMat;
    glm::vec3 eye; // Camera position in 3D
    glm::vec3 lookAt; // Point that the camera is looking at
    glm::vec3 up; // Orientation of the camera

    glm::vec2 screenSize;
    bool dragging = false;
};
