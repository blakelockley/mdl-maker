#include "controls.h"

#include <stdio.h>

#include "array.h"
#include "camera.h"
#include "filemanager.h"
#include "guide.h"
#include "light.h"
#include "linmath.h"
#include "object.h"

int shift_pressed = 0;

int selection_len = 0;
int selection_buffer[256];

int light_selected = 0;

int show_points = 1;
int show_lines = 0;
extern int width, height;

extern char *filename;
extern object_t object;
extern camera_t camera;
extern light_t light;
extern guide_t guide;

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

    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        toggle_camera_projection();

    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        set_scroll(0.0f);

    if (key == GLFW_KEY_MINUS && (action == GLFW_PRESS || action == GLFW_REPEAT))
        update_zoom(0.5f);

    if (key == GLFW_KEY_EQUAL && (action == GLFW_PRESS || action == GLFW_REPEAT))
        update_zoom(-0.5f);

    if (key == GLFW_KEY_A && action == GLFW_PRESS && !mods) {
        if (!shift_pressed)
            selection_len = 0;

        add_point_selection(&object);
    }

    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        show_lines = !show_lines;

    if (key == GLFW_KEY_P && action == GLFW_PRESS)
        show_points = !show_points;

    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        add_face_selection(&object);

    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        vec3 midpoint;
        get_selection_midpoint(midpoint, &object);
        vec3_add(midpoint, midpoint, (vec3){0.0f, 0.01f, 0.0f});

        move_selection(&object, midpoint);
    }

    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        vec3 midpoint;
        get_selection_midpoint(midpoint, &object);
        vec3_add(midpoint, midpoint, (vec3){0.0f, -0.01f, 0.0f});

        move_selection(&object, midpoint);
    }

    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        vec3 midpoint;
        get_selection_midpoint(midpoint, &object);

        vec3 dir;
        if (fabs(camera.right[0]) >= fabs(camera.right[2])) {
            if (camera.right[0] >= 0.0f)
                vec3_set(dir, 0.01f, 0.0f, 0.0f);
            else
                vec3_set(dir, -0.01f, 0.0f, 0.0f);
        } else {
            if (camera.right[2] >= 0.0f)
                vec3_set(dir, 0.0f, 0.0f, 0.01f);
            else
                vec3_set(dir, 0.0f, 0.0f, -0.01f);
        }

        vec3_add(midpoint, midpoint, dir);

        move_selection(&object, midpoint);
    }

    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        vec3 midpoint;
        get_selection_midpoint(midpoint, &object);

        vec3 dir;
        if (fabs(camera.right[0]) >= fabs(camera.right[2])) {
            if (camera.right[0] >= 0.0f)
                vec3_set(dir, -0.01f, 0.0f, 0.0f);
            else
                vec3_set(dir, 0.01f, 0.0f, 0.0f);
        } else {
            if (camera.right[2] >= 0.0f)
                vec3_set(dir, 0.0f, 0.0f, -0.01f);
            else
                vec3_set(dir, 0.0f, 0.0f, 0.01f);
        }

        vec3_add(midpoint, midpoint, dir);

        move_selection(&object, midpoint);
    }

    if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
        if (selection_len == 0)
            return;

        remove_selection(&object);
        selection_len = 0;
    }

    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
        shift_pressed = 1;

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        if (light_selected)
            set_light_position((vec3){0.0f, 1.0f, 0.0f});
        else
            guide.is_visible = 0;
    }

    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
        shift_pressed = 0;

    if (key == GLFW_KEY_S && action == GLFW_PRESS && mods == GLFW_MOD_SUPER)
        save_file(filename, &object);

    if (key == GLFW_KEY_A && action == GLFW_PRESS && mods == GLFW_MOD_SUPER)
        select_all(&object);

    if (key == GLFW_KEY_D && action == GLFW_PRESS)
        duplicate_selection(&object);

    if (key == GLFW_KEY_M && action == GLFW_PRESS)
        mirror_selection(&object);
}

void character_callback(GLFWwindow *window, unsigned int codepoint) {
    // if ('a' <= codepoint && codepoint <= 'z')
    //     current_command = codepoint - 32;
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS) {
        int w, h;
        glfwGetWindowSize(window, &w, &h);

        set_ray(xpos, ypos, w, h);

        vec3 midpoint;
        if (light_selected)
            vec3_copy(midpoint, light.pos);
        else if (selection_len > 0)
            get_selection_midpoint(midpoint, &object);

        vec3 p0, n, q;
        vec3_copy(p0, midpoint);
        vec3_cross(n, camera.up, camera.right);
        vec3_sub(q, p0, camera.ray_start);

        float t = vec3_dot(q, n) / vec3_dot(camera.ray, n);

        vec3 ray;
        vec3_scale(ray, camera.ray, t);
        vec3_add(ray, ray, camera.ray_start);

        if (light_selected)
            set_light_position(ray);
        else if (selection_len > 0)
            move_selection(&object, ray);
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        int w, h;
        glfwGetWindowSize(window, &w, &h);

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        set_ray(xpos, ypos, w, h);

        light_selected = check_light_intercept();
        if (light_selected)
            return;

        int vertex = find_intercept(&object);
        if (shift_pressed) {
            if (vertex == -1)
                return;

            int index = find_index(selection_buffer, selection_len, vertex);
            if (index == -1)
                selection_buffer[selection_len++] = vertex;
            else
                remove_index(selection_buffer, &selection_len, index);
        } else {
            selection_len = 0;
            if (vertex >= 0)
                selection_buffer[selection_len++] = vertex;
        }
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    if (guide.is_visible) {
        vec3 midpoint, delta;
        get_selection_midpoint(midpoint, &object);

        vec3_scale(delta, guide.axis, yoffset < 0.0f ? 0.01f : -0.01f);
        vec3_add(midpoint, midpoint, delta);

        move_selection(&object, midpoint);

    } else {
        update_scroll(xoffset * (shift_pressed ? 0.1f : 0.5f));

        if (shift_pressed)
            update_angle(yoffset * 0.25f);
    }
}