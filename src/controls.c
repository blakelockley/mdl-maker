#include "controls.h"

#include <stdlib.h>
#include "linmath.h"

#include "macros.h"
#include "camera.h"

extern struct ImGuiIO* io;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (io->WantCaptureKeyboard)
        return;
    
    if (key == GLFW_KEY_W && action == GLFW_PRESS && mods == GLFW_MOD_SUPER)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (io->WantCaptureMouse)
        return;

    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    if (io->WantCaptureMouse)
        return;

    double delta_x = io->MouseDelta.x;
    double delta_y = io->MouseDelta.y;
    
    if (!glfwGetWindowAttrib(window, GLFW_FOCUSED))
        glfwFocusWindow(window);

    if (fabs(delta_x) > 100.0f || fabs(delta_y) > 100.0f)
        return; // discard action when delta is too large

    // Euclidan translation
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        update_camera_origin(delta_x, delta_y);
    
    // Orbital rotation
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        update_camera_orbit(delta_x, delta_y);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    if (io->WantCaptureMouse)
        return;
    
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // no mouse
        update_camera_orbit(xoffset * 2.0f, yoffset * 2.0f);
    else
        update_camera_radius(yoffset);
}
