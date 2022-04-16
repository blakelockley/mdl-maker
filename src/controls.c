#include "controls.h"

#include <stdio.h>

#include "linmath.h"
#include "camera.h"

extern camera_t camera;

float scroll = 0.0f;

int show_points = 1;
int show_lines = 0;

void normalize_mouse_pos(double *normal_x, double *normal_y, double mouse_x, double mouse_y, int width, int height) {
    *normal_x = (2.0f * mouse_x) / width - 1.0f;
    *normal_y = 1.0f - (2.0f * mouse_y) / height;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    scroll += xoffset;
    vec3_set(camera.pos, -sinf(scroll) * 2, camera.pos[1], cosf(scroll) * 2);
}
