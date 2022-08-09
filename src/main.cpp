#include "glfw_app.hpp"
#include "gui/gui.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "Mesh.hpp"


#include "mathutils/ArcBall.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0,0,0), glm::vec3(0,1,0));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

CArcBall arcBall;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

class Application : public GLFWApplication
{
private:
    IgnisShader shader;
    Mesh* mesh;


    bool show_demo_window = true;
public:
    Application() : GLFWApplication("Application", SCR_WIDTH, SCR_HEIGHT, true)
    {
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);

        arcBall.place(CPosition(width / 2, height / 2), 0.5 * sqrt(double(width * width + height * height)));

        gui_init(window, "#version 130");

        ignisCreateShadervf(&shader, "res/shaders/shader.vert", "res/shaders/shader.frag");

        mesh = Mesh::loadObj("res/cube.obj");

        // draw in wireframe
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    ~Application()
    {
        delete mesh;
        ignisDeleteShader(&shader);

        gui_shutdown();
    }

    void update(float deltaTime)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

    }

    void render()
    {
        // don't forget to enable shader before setting uniforms
        ignisUseShader(&shader);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ignisSetUniformMat4(&shader, "projection", &projection[0][0]);

        CMatrix matModel = arcBall.getRotatonMatrix().getInverse();

        double matrix[16];
        matModel.get(matrix[0], matrix[4], matrix[8], matrix[12],
            matrix[1], matrix[5], matrix[9], matrix[13],
            matrix[2], matrix[6], matrix[10], matrix[14],
            matrix[3], matrix[7], matrix[11], matrix[15]);

        //ignisSetUniformMat4(&shader, "view", (float*)matrix);
        ignisSetUniformMat4(&shader, "view", &view[0][0]);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        ignisSetUniformMat4(&shader, "model", &model[0][0]);

        mesh->render();

        gui_start_frame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        gui_render();
    }

};

int main()
{
    Application* app = new Application();

    app->run();

    delete app;
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xPos = static_cast<float>(xposIn);
    float yPos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    // Get the homogenous position of the camera and pivot point
    glm::vec4 position(camera.GetEye().x, camera.GetEye().y, camera.GetEye().z, 1);
    glm::vec4 pivot(camera.GetLookAt().x, camera.GetLookAt().y, camera.GetLookAt().z, 1);

    // step 1 : Calculate the amount of rotation given the mouse movement.
    float deltaAngleX = (2 * glm::pi<float>() / SCR_WIDTH); // a movement from left to right = 2*PI = 360 deg
    float deltaAngleY = (glm::pi<float>() / SCR_HEIGHT);  // a movement from top to bottom = PI = 180 deg
    float xAngle = (lastX - xPos) * deltaAngleX;
    float yAngle = (lastY - yPos) * deltaAngleY;

    // Extra step to handle the problem when the camera direction is the same as the up vector
    float cosAngle = dot(camera.GetViewDir(), camera.GetUpVector());
    if (cosAngle * glm::sign(deltaAngleY) > 0.99f)
        deltaAngleY = 0;

    // step 2: Rotate the camera around the pivot point on the first axis.
    glm::mat4x4 rotationMatrixX(1.0f);
    rotationMatrixX = glm::rotate(rotationMatrixX, xAngle, camera.GetUpVector());
    position = (rotationMatrixX * (position - pivot)) + pivot;

    // step 3: Rotate the camera around the pivot point on the second axis.
    glm::mat4x4 rotationMatrixY(1.0f);
    rotationMatrixY = glm::rotate(rotationMatrixY, yAngle, camera.GetRightVector());
    glm::vec3 finalPosition = (rotationMatrixY * (position - pivot)) + pivot;

    // Update the camera view (we keep the same lookat and the same up vector)
    camera.SetCameraView(finalPosition, camera.GetLookAt(), camera.GetUpVector());

    lastX = xPos;
    lastY = yPos;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // camera.ProcessMouseScroll(static_cast<float>(yoffset));
}