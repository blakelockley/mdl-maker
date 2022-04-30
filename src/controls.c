#include "controls.h"

#include "linmath.h"
#include "camera.h"
#include "select.h"
#include "model.h"

extern camera_t camera;


void normalize_mouse_pos(double *normal_x, double *normal_y, double mouse_x, double mouse_y, int width, int height) {
    *normal_x = (2.0f * mouse_x) / width - 1.0f;
    *normal_y = 1.0f - (2.0f * mouse_y) / height;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        uint32_t index = add_vertex((vec3){0.0f, 0.5f, 0.0f});
        
        clear_selection();
        add_index_to_selection(index);
    }
    
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        if (mods & GLFW_MOD_SHIFT)
            flip_face();
        
        else
            add_face();
    }
    
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        vec3 offset;
        if (fabs(camera.right[0]) >= fabs(camera.right[2])) {
            if (camera.right[0] >= 0.0f)
                vec3_set(offset, 0.01f, 0.0f, 0.0f);
            else
                vec3_set(offset, -0.01f, 0.0f, 0.0f);
        } else {
            if (camera.right[2] >= 0.0f)
                vec3_set(offset, 0.0f, 0.0f, 0.01f);
            else
                vec3_set(offset, 0.0f, 0.0f, -0.01f);
        }
        move_selection(offset);
    }
    
    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        vec3 offset;
        if (fabs(camera.right[0]) >= fabs(camera.right[2])) {
            if (camera.right[0] >= 0.0f)
                vec3_set(offset, -0.01f, 0.0f, 0.0f);
            else
                vec3_set(offset, +0.01f, 0.0f, 0.0f);
        } else {
            if (camera.right[2] >= 0.0f)
                vec3_set(offset, 0.0f, 0.0f, -0.01f);
            else
                vec3_set(offset, 0.0f, 0.0f, +0.01f);
        }
        move_selection(offset);
    }

    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        vec3 offset;
        vec3_set(offset, 0.0f, +0.01f, 0.0f);
        move_selection(offset);
    }

    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        vec3 offset;
        vec3_set(offset, 0.0f, -0.01f, 0.0f);
        move_selection(offset);
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    static float scroll = 0.0f;
    scroll += xoffset;
    
    set_camera_position(&camera, (vec3){ -sinf(scroll) * 2, camera.pos[1], cosf(scroll) * 2});
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
