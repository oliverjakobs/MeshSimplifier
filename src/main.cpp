#include "glfw_app.hpp"
#include "gui/gui.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "Mesh.hpp"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mod);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0,0,0), glm::vec3(0,1,0));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

class Application : public GLFWApplication
{
private:
    IgnisShader shader;
    Mesh* mesh;

    bool showWireframe = false;
    bool show_demo_window = true;
public:
    Application() : GLFWApplication("Application", SCR_WIDTH, SCR_HEIGHT, true)
    {
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, mouse_move_callback);

        camera.setScreenSize((float)width, (float)height);

        gui_init(window, "#version 130");

        ignisCreateShadervf(&shader, "res/shaders/shader.vert", "res/shaders/shader.frag");

        mesh = Mesh::loadObj("res/cube.obj");
        //mesh = Mesh::loadObj("res/Marsienne_Base.obj");
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

        glm::vec3 lightPos(0.0f, 2.0f, 0.0f);
        ignisSetUniform3f(&shader, "lightPos", &lightPos[0]);
        ignisSetUniform3f(&shader, "viewPos", &camera.getPosition()[0]);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ignisSetUniformMat4(&shader, "projection", &projection[0][0]);
        ignisSetUniformMat4(&shader, "view", &view[0][0]);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        ignisSetUniformMat4(&shader, "model", &model[0][0]);

            // draw in wireframe
        if (showWireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        mesh->render();

        gui_start_frame();

        ImGui::Begin("Info");
        ImGui::Checkbox("Wireframe", &showWireframe);

        ImGui::End();

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


void mouse_button_callback(GLFWwindow* window, int button, int action, int mod)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)   camera.startDragging();
        else                        camera.stopDragging();
    }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_move_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xPos = static_cast<float>(xposIn);
    float yPos = static_cast<float>(yposIn);

    camera.updateMouse(xPos, yPos, lastX, lastY);

    lastX = xPos;
    lastY = yPos;
}