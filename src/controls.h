#ifndef CONTROLS_H
#define CONTROLS_H

#include "glfw.h"

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

#endif  // CONTROLS_H