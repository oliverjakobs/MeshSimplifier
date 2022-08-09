#pragma once

#include "imgui.h"

#include "GLFW/glfw3.h"

void gui_init(GLFWwindow* context, const char* glsl_version);
void gui_shutdown();

void gui_start_frame();
void gui_render();