#ifndef GUI_H
#define GUI_H

#include <stdbool.h>
#include "glfw.h"

// ImGui structs

struct ImGuiContext* ctx;
struct ImGuiIO* io;

// GUI methods

void gui_init(GLFWwindow *window);
void gui_terminate();
void gui_update();
void gui_render();

#endif  // GUI_H