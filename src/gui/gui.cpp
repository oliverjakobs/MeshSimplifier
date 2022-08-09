#include "gui.hpp"

#include "imgui_impl_glfw.hpp"
#include "imgui_impl_opengl3.hpp"

void gui_init(GLFWwindow* context, const char* glsl_version)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    io.WantCaptureMouse = true;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(context, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void gui_shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void gui_start_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void gui_render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
