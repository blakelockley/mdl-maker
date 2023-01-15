#include "controls.h"

#include <stdlib.h>
#include "linmath.h"

#include "macros.h"
#include "camera.h"
#include "selection.h"
#include "model.h"
#include "picker.h"
#include "file.h"

extern camera_t camera;
extern selection_t selection;
extern model_t model;
extern picker_t picker;

extern struct ImGuiIO* io;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (io->WantCaptureKeyboard)
        return;
    
    // TODO: Check for changes and show save dialog before closing
    if (key == GLFW_KEY_W && action == GLFW_PRESS && mods == GLFW_MOD_SUPER)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    
    if (key == GLFW_KEY_S && action == GLFW_PRESS && mods == GLFW_MOD_SUPER)
        save_file(NULL, &model);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        clear_selection(&selection);
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (io->WantCaptureMouse)
        return;

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        vec3 vertex = { 0.0f, 0.0f, 0.0f };
        
        // TODO: Add project to plane calculations
        
        add_vertex(&model, vertex);
    }
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
        update_origin(&camera, delta_x, delta_y);
    
    // Orbital rotation
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        update_orbit(&camera, delta_x, delta_y);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    if (io->WantCaptureMouse)
        return;
    
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // no mouse
        update_orbit(&camera, xoffset * 2.0f, yoffset * 2.0f);
    else
        update_radius(&camera, yoffset);
}
