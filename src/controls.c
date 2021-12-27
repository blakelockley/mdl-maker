#include "controls.h"

#include "camera.h"
#include "linmath.h"
#include "model.h"

int shift_pressed = 0;

int selection_len = 0;
int selection_buffer[256];

extern model_t object;
extern camera_t camera;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if ((key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE) && action == GLFW_PRESS)
        selection_len = 0;

    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        set_scroll(0.0f);

    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        set_scroll(90 * M_PI / 180);

    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
        set_scroll(180 * M_PI / 180);

    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
        set_scroll(270 * M_PI / 180);

    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
        toggle_camera_view();

    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        set_scroll(0.0f);

    if (key == GLFW_KEY_MINUS && (action == GLFW_PRESS || action == GLFW_REPEAT))
        update_zoom(0.5f);

    if (key == GLFW_KEY_EQUAL && (action == GLFW_PRESS || action == GLFW_REPEAT))
        update_zoom(-0.5f);

    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        add_vertex(&object, (vec3){0.0f, 1.0f, 0.0f});

        selection_len = 0;
        selection_buffer[selection_len++] = object.vertices_len - 1;
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        add_face(&object);

    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        vec3 vector;
        vec3_scale(vector, camera.right, -(shift_pressed ? 0.01f : 0.1f));

        move_selection(&object, vector);
    }

    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        vec3 vector;
        vec3_scale(vector, camera.right, (shift_pressed ? 0.01f : 0.1f));

        move_selection(&object, vector);
    }

    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        vec3 vector;
        vec3_scale(vector, camera.up, -(shift_pressed ? 0.01f : 0.1f));

        move_selection(&object, vector);
    }

    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        vec3 vector;
        vec3_scale(vector, camera.up, (shift_pressed ? 0.01f : 0.1f));

        move_selection(&object, vector);
    }

    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
        shift_pressed = 1;

    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
        shift_pressed = 0;
}

void character_callback(GLFWwindow *window, unsigned int codepoint) {
    // if ('a' <= codepoint && codepoint <= 'z')
    //     current_command = codepoint - 32;
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        int w, h;
        glfwGetWindowSize(window, &w, &h);

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        set_ray(xpos, ypos, w, h);
        int index = find_intercept(&object);

        int already_selected = 0;
        for (int i = 0; i < selection_len; i++)
            if (selection_buffer[i] == index) {
                already_selected = 1;
                break;
            }

        if (already_selected) {
            int j = 0;
            for (int i = 0; i < selection_len; i++)
                if (selection_buffer[i] != index)
                    selection_buffer[j++] = selection_buffer[i];

            selection_len--;
        } else {
            if (!shift_pressed)
                selection_len = 0;

            selection_buffer[selection_len++] = index;
        }
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    update_scroll(xoffset * (shift_pressed ? 0.1f : 0.5f));
}