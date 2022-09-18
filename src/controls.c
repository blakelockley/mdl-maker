#include "controls.h"

#include "linmath.h"
#include "gui/gui.h"

#include "camera.h"
#include "macros.h"

extern camera_t camera;

extern struct ImGuiIO* io;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (io->WantCaptureKeyboard)
        return;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (io->WantCaptureMouse)
        return;
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    if (io->WantCaptureMouse)
        return;
    
    double delta_x =  io->MouseDelta.x;
    double delta_y =  io->MouseDelta.y;

    // Orbital rotation
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE
     && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
        update_orbit(&camera, delta_x, delta_y);

    // Euclidan translation
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS
     && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {

        vec3 right;
        vec3_cross(right, camera.dir, camera.up);
        vec3_normalize(right, right);
        
        vec3 xz_vector;
        vec3_scale(xz_vector, right, delta_x * -0.01f);
        vec3_add(camera.pos, camera.pos, xz_vector);

        camera.pos[1] += delta_y * 0.01f;
    }

}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    if (io->WantCaptureMouse)
        return;

    float delta = yoffset * -0.1f;
    
    vec3 vector;
    vec3_scale(vector, camera.dir, delta);

    vec3 new_pos;
    vec3_add(new_pos, camera.pos, vector);

    vec3 old_pos;
    vec3_copy(old_pos, camera.pos);

    // Check that change is still +'ve
    if (new_pos[0] / old_pos[0] > 0.0f
     && new_pos[1] / old_pos[1] > 0.0f
     && new_pos[2] / old_pos[2] > 0.0f)
        vec3_copy(camera.pos, new_pos);
}
