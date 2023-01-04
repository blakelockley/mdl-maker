#include "controls.h"

#include <stdlib.h>
#include "linmath.h"

#include "macros.h"
#include "camera.h"
#include "selection.h"
#include "model.h"
#include "picker.h"


extern camera_t camera;
extern selection_t selection;
extern model_t model;
extern picker_t picker;

extern struct ImGuiIO* io;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (io->WantCaptureKeyboard)
        return;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        selection.len = 0;
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (io->WantCaptureMouse)
        return;

    float mouse_x = io->MousePos.x;
    float mouse_y = io->MousePos.y;

    bool shift_pressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        handle_selection_start(&selection, mouse_x, mouse_y, shift_pressed);
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        handle_selection_end(&selection, mouse_x, mouse_y, shift_pressed);
    
    // TODO: Move logic into selection/face picker
    // if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    //     uint32_t picked_face_index = render_picker_to_face_id(&picker, &model);
        
    //     if (!shift_pressed)
    //         clear_selection(&selection);
        
    //     if (picked_face_index != INDEX_NOT_FOUND)
    //         append_selection(&selection, picked_face_index);
    // }
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    if (io->WantCaptureMouse)
        return;

    float mouse_x = io->MousePos.x;
    float mouse_y = io->MousePos.y;

    double delta_x = io->MouseDelta.x;
    double delta_y = io->MouseDelta.y;

    bool shift_pressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

    // Selection
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        handle_selection_move(&selection, mouse_x, mouse_y, shift_pressed);

    // Euclidan translation
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && !shift_pressed)
        update_position(&camera, delta_x, delta_y);
    
    // Orbital rotation
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS && !shift_pressed)
        update_orbit(&camera, delta_x, delta_y);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    if (io->WantCaptureMouse)
        return;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        update_orbit(&camera, xoffset * 2.0f, yoffset * 2.0f);
    else
        update_depth(&camera, yoffset);
}
