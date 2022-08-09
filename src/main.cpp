
#include "glfw_app.hpp"
#include "gui/gui.hpp"

#include "Mesh.hpp"

#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

class Application : public GLFWApplication
{
private:
    Mesh* mesh;
    IgnisShader shader;

    bool show_demo_window = true;

public:
    Application() : GLFWApplication()
    {
        gui_init(window, "#version 130");

        ignisCreateShadervf(&shader, "res/shaders/shader.vert", "res/shaders/shader.frag");

        mesh = Mesh::loadObj("res/cube.obj");

        glViewport(0, 0, (GLsizei)m_nWidth, (GLsizei)m_nHeight);
    }

    ~Application()
    {
        delete mesh;

        ignisDeleteShader(&shader);
        gui_shutdown();
    }


    void render()
    {
        ignisUseShader(&shader);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)m_nWidth / (float)m_nHeight, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down

        ignisSetUniformMat4(&shader, "projection", &projection[0][0]);
        ignisSetUniformMat4(&shader, "view", &view[0][0]);
        ignisSetUniformMat4(&shader, "model", &model[0][0]);


        mesh->render();

        gui_start_frame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        gui_render();
    }

};

int main(int argc, char* argv[])
{
    Application* app = new Application();

    app->run();

    delete app;
    return 0;
}
