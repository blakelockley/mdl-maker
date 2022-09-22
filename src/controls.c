#include "controls.h"

#include "linmath.h"
#include "gui.h"

#include "macros.h"
#include "camera.h"
#include "selection.h"
#include "model.h"

extern camera_t camera;
extern selection_t selection;
extern model_t model;

extern uint8_t mode;

extern struct ImGuiIO* io;

void normalize_mouse_pos(GLFWwindow *window, float *normal_x, float *normal_y, float mouse_x, float mouse_y) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    *normal_x = (2.0f * mouse_x) / width - 1.0f;
    *normal_y = 1.0f - (2.0f * mouse_y) / height;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (io->WantCaptureKeyboard)
        return;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (io->WantCaptureMouse)
        return;

    float mouse_x = io->MousePos.x;
    float mouse_y = io->MousePos.y;

    float clip_x, clip_y;
    normalize_mouse_pos(window, &clip_x, &clip_y, mouse_x, mouse_y);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        handle_selection_start(&selection, clip_x, clip_y);    

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        handle_selection_end(&selection, clip_x, clip_y, mods & GLFW_MOD_SHIFT);    
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    if (io->WantCaptureMouse)
        return;

    double delta_x = io->MouseDelta.x;
    double delta_y = io->MouseDelta.y;

    float clip_x, clip_y;
    normalize_mouse_pos(window, &clip_x, &clip_y, xpos, ypos);

    // Selection
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        handle_selection_move(&selection, clip_x, clip_y);

    // Euclidan translation
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS
     && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
        update_position(&camera, delta_x, delta_y);
    
    // Orbital rotation
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE
     && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
        update_orbit(&camera, delta_x, delta_y);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    if (io->WantCaptureMouse)
        return;

    if (mode == MODE_SELECT)
        update_depth(&camera, yoffset);

    float delta = yoffset * 0.01f;

    vec3 vec_delta;
    vec3_scale(vec_delta, selection.control_axis, delta);

    if (mode == MODE_TRANSLATE)
        move_vertices(&model, selection.indices, selection.len, vec_delta);

    if (mode == MODE_ROTATE)
        rotate_vertices(&model, selection.indices, selection.len, mode_axis, delta);

    if (mode == MODE_SCALE)
        scale_vertices(&model, selection.indices, selection.len, delta);
}
