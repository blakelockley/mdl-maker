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

    float mouse_x = io->MousePos.x;
    float mouse_y = io->MousePos.y;

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    if (mouse_x < 0 || mouse_x > width || mouse_y < 0 || mouse_y > height)
        return; // discard action when click is outside of window

    bool shift_pressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        handle_selection_start(&selection, mouse_x, mouse_y, shift_pressed);
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        handle_selection_end(&selection, mouse_x, mouse_y, shift_pressed);
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    if (io->WantCaptureMouse)
        return;

    float mouse_x = io->MousePos.x;
    float mouse_y = io->MousePos.y;

    double delta_x = io->MouseDelta.x;
    double delta_y = io->MouseDelta.y;
    
    if (!glfwGetWindowAttrib(window, GLFW_FOCUSED))
        glfwFocusWindow(window);

    if (fabs(delta_x) > 100.0f || fabs(delta_y) > 100.0f)
        return; // discard action when delta is too large

    bool shift_pressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

    // Selection
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        handle_selection_move(&selection, mouse_x, mouse_y, shift_pressed);

    // Euclidan translation
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS && shift_pressed)
        update_origin(&camera, delta_x, delta_y);
    
    // Orbital rotation
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS && !shift_pressed)
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
