#include <Ignis/Ignis.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Gui.hpp"

#include "Mesh.hpp"
#include "MeshSimplifier.hpp"
#include "App.hpp"

#include "Camera.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mod);
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

glm::vec3 cameraPosition(0.0f, 0.0f, 5.0f);
glm::vec3 objectPosition(0.0f);

// camera
Camera camera(cameraPosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

// mesh
//MeshData data("res/Marsienne_Base.obj");
MeshData data("res/monkey.obj");
//MeshData data("res/cube.obj");

class Application : public GLFWApplication
{
private:
    IgnisShader shader;

    Mesh* mesh;
    MeshSimplifier* simplifier;

    bool showWireframe = false;
    bool cullBackFaces = false;
    int targetFaces = 0;
public:
    Application() : GLFWApplication("Application", SCR_WIDTH, SCR_HEIGHT, true)
    {
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, mouse_move_callback);

        camera.setScreenSize((float)SCR_WIDTH, (float)SCR_HEIGHT);

        gui_init(window, "#version 130");

        ignisCreateShadervf(&shader, "res/shaders/shader.vert", "res/shaders/shader.frag");

        simplifier = new MeshSimplifier(data.vertices, data.indices);
        mesh = new Mesh(simplifier->getVertices(), simplifier->getIndices());

        targetFaces = simplifier->getFaceCount();
    }

    ~Application()
    {
        delete mesh;
        delete simplifier;

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
        ignisUseShader(&shader);
        ignisSetUniform3f(&shader, "lightPos", &camera.getPosition()[0]);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), camera.getAspectRatio(), 0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();
        ignisSetUniformMat4(&shader, "projection", &projection[0][0]);
        ignisSetUniformMat4(&shader, "view", &view[0][0]);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, objectPosition);
        ignisSetUniformMat4(&shader, "model", &model[0][0]);

        // draw in wireframe
        if (showWireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (cullBackFaces)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }

        // render the cube
        mesh->render();

        gui_start_frame();

        //ImGui::ShowDemoWindow(NULL);

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(200, camera.getHeight()));
        ImGui::Begin("Info", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        ImGui::Text("Mesh info:");
        ImGui::Text("Vertices: %d", simplifier->getVertexCount());
        ImGui::Text("Faces:    %d", simplifier->getFaceCount());
        ImGui::Separator();

        ImGui::Text("Simplifier:");

        ImGui::SliderInt("Target Faces", &targetFaces, 0, simplifier->getFaceCount());

        if (ImGui::Button("Simplify"))
        {
            simplifier->run(targetFaces);
            mesh->reload(simplifier->getVertices(), simplifier->getIndices());
            printf("Mesh simplified (%zd faces).\n", simplifier->getFaceCount());
        }

        if (ImGui::Button("Reset"))
        {
            simplifier->reload(data.vertices, data.indices);
            mesh->reload(simplifier->getVertices(), simplifier->getIndices());
            targetFaces = simplifier->getFaceCount();
            printf("Mesh reset.\n");
        }

        ImGui::Separator();

        ImGui::Text("Render settings:");

        ImGui::Checkbox("Wireframe", &showWireframe);
        ImGui::Checkbox("Cull Backfaces", &cullBackFaces);

        ImGui::End();

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    camera.setScreenSize((float)width, (float)height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mod)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)   camera.startDragging();
        else                        camera.stopDragging();
    }
}

void mouse_move_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xPos = static_cast<float>(xposIn);
    float yPos = static_cast<float>(yposIn);

    if (!ImGui::GetIO().WantCaptureMouse)
        camera.updateMouse(xPos, yPos, lastX, lastY);

    lastX = xPos;
    lastY = yPos;
}