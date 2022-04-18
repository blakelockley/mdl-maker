#include "controls.h"

#include <stdio.h>

#include "linmath.h"
#include "camera.h"
#include "select.h"
#include "model.h"

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
    
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        add_vetex((vec3){0.0f, 0.5f, 0.0f});
    
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        move_selection((vec3){0.1f, 0.0f, 0.0f});
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    scroll += xoffset;
    vec3_set(camera.pos, -sinf(scroll) * 2, camera.pos[1], cosf(scroll) * 2);
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    int w, h;
    glfwGetWindowSize(window, &w, &h);

    double normal_x, normal_y;
    normalize_mouse_pos(&normal_x, &normal_y, xpos, ypos, w, h);

    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS)
        set_select_move(normal_x, normal_y);
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    int w, h;
    glfwGetWindowSize(window, &w, &h);

    double x, y;
    glfwGetCursorPos(window, &x, &y);
   
    double normal_x, normal_y;
    normalize_mouse_pos(&normal_x, &normal_y, x, y, w, h);
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS)
            set_select_start(normal_x, normal_y);
        else if (action == GLFW_RELEASE)
            set_select_end(normal_x, normal_y);
    }
}
