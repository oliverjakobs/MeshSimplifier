#include "App.hpp"

#include "Mesh.hpp"
#include "MeshSimplifier.hpp"

#include "Camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
const char* models[] = { "Marsienne_Base", "monkey", "cube" };
std::string getModelPath(int i) { return "res/" + std::string(models[i]) + ".obj"; }

int currentModel = 1;
MeshData data(getModelPath(currentModel));

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
        camera.setScreenSize((float)SCR_WIDTH, (float)SCR_HEIGHT);

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
    }

    void onUpdate(float deltaTime)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    void onRender()
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

        // enable/disable wireframe mode
        if (showWireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // enable/disable backface culling
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
    }

    void onRenderGui()
    {
        // ImGui::ShowDemoWindow(NULL);

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(200, camera.getHeight()));
        ImGui::Begin("Info", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Vertices: %d", simplifier->getVertexCount());
            ImGui::Text("Faces:    %d", simplifier->getFaceCount());

            ImGui::Separator();

            ImGui::Text("Load model:");

            ImGui::Combo("##model", &currentModel, models, IM_ARRAYSIZE(models));

            ImGui::SameLine();

            if (ImGui::Button("Load", ImVec2(-FLT_MIN, 0.0f)))
            {
                data = MeshData(getModelPath(currentModel));
                simplifier->reload(data.vertices, data.indices);
                delete mesh;
                mesh = new Mesh(simplifier->getVertices(), simplifier->getIndices());
                targetFaces = simplifier->getFaceCount();
            }

            ImGui::Dummy(ImVec2(0.0f, 16.0f));
        }

        if (ImGui::CollapsingHeader("Simplifier", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Target Faces:");
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::SliderInt("##faces", &targetFaces, 0, simplifier->getFaceCount());

            float buttonWidth = ImGui::GetContentRegionAvail().x * 0.5f;
            if (ImGui::Button("Simplify", ImVec2(buttonWidth, 0.0f)))
            {
                simplifier->run(targetFaces);
                mesh->reload(simplifier->getVertices(), simplifier->getIndices());
                targetFaces = simplifier->getFaceCount();
                printf("Mesh simplified (%zd faces).\n", simplifier->getFaceCount());
            }

            ImGui::SameLine();

            if (ImGui::Button("Reset", ImVec2(-FLT_MIN, 0.0f)))
            {
                simplifier->reload(data.vertices, data.indices);
                mesh->reload(simplifier->getVertices(), simplifier->getIndices());
                targetFaces = simplifier->getFaceCount();
                printf("Mesh reset.\n");
            }

            ImGui::Dummy(ImVec2(0.0f, 16.0f));
        }

        if (ImGui::CollapsingHeader("Render settings", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Checkbox("Wireframe", &showWireframe);
            ImGui::Checkbox("Cull Backfaces", &cullBackFaces);
        }

        ImGui::End();
    }

    void onResize(int width, int height)
    {
        glViewport(0, 0, width, height);
        camera.setScreenSize((float)width, (float)height);
    }

    void onMouseButton(int button, int action)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (action == GLFW_PRESS)   camera.startDragging();
            else                        camera.stopDragging();
        }
    }

    void onMouseMove(float xPos, float yPos)
    {
        if (!ImGui::GetIO().WantCaptureMouse)
            camera.updateMouse(xPos, yPos, lastX, lastY);

        lastX = xPos;
        lastY = yPos;
    }
};

int main()
{
    Application app;
    app.run();

    return 0;
}
