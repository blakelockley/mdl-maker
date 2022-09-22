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

// Modes

#define MODE_SELECT    0b0000
#define MODE_TRANSLATE 0b0001
#define MODE_ROTATE    0b0010
#define MODE_SCALE     0b0100

#define MODE_AXIS_NONE 0b0000
#define MODE_AXIS_X    0b0001
#define MODE_AXIS_Y    0b0010
#define MODE_AXIS_Z    0b0100

uint8_t mode, mode_axis;

#endif  // GUI_H