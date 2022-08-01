#include "controls.h"
#include "linmath.h"
#include "camera.h"
#include "selection.h"
#include "model.h"
#include "file.h"

#include <stdio.h>

extern int show_fps;
extern bool is_open;

extern camera_t camera;
extern selection_t selection;
extern model_t model;

extern char *filename;

void normalize_mouse_pos(double *normal_x, double *normal_y, double mouse_x, double mouse_y, int width, int height) {
    *normal_x = (2.0f * mouse_x) / width - 1.0f;
    *normal_y = 1.0f - (2.0f * mouse_y) / height;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    int shift_pressed = (mods & GLFW_MOD_SHIFT);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (key == GLFW_KEY_SLASH && action == GLFW_PRESS && shift_pressed)
        show_fps = !show_fps;
    
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        uint32_t index = add_vertex(&model, (vec3){0.0f, 0.5f, 0.0f});
        
        clear_selection(&selection);
        extend_selection(&selection, index);
    }
    
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
        is_open = true;
    
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
        extend_face(&model, selection.indices, selection.len);
    
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
        duplicate_vertices(&model, selection.indices, selection.len);
    
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        if (shift_pressed) {
            face_t *face = get_face(&model, selection.indices, selection.len);
            flip_face(&model, face);
        } else {
            add_face(&model, selection.indices, selection.len);
        }
    }

    if (key == GLFW_KEY_M && action == GLFW_PRESS)
        set_render_mode(&model, RENDER_MODE_FACES);

    if (key == GLFW_KEY_N && action == GLFW_PRESS)
        toggle_render_mode(&model, RENDER_MODE_NORMALS);

    if (key == GLFW_KEY_S && action == GLFW_PRESS && (mods & GLFW_MOD_SUPER))
        save_file(filename, &model);

    if (key == GLFW_KEY_V && action == GLFW_PRESS)
        toggle_render_mode(&model, RENDER_MODE_VERTICES);

    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        set_render_mode(&model, RENDER_MODE_WIREFRAME);
    
    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        remove_vertices(&model, selection.indices, selection.len);
    }
    
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        if (shift_pressed) {
            scale_vertices(&model, selection.indices, selection.len, 1.01f);
        } else {
            vec3 heading;
            get_camera_heading(&camera, heading);
            
            vec3 offset;
            vec3_cross(offset, heading, (vec3){0.0f, 1.0f, 0.0f});
            vec3_scale(offset, offset, +0.01f);

            move_vertices(&model, selection.indices, selection.len, offset);
        }
    }
    
    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        if (shift_pressed) {
            scale_vertices(&model, selection.indices, selection.len, 0.99f);
        } else {
            vec3 heading;
            get_camera_heading(&camera, heading);
            
            vec3 offset;
            vec3_cross(offset, heading, (vec3){0.0f, 1.0f, 0.0f});
            vec3_scale(offset, offset, -0.01f);

            move_vertices(&model, selection.indices, selection.len, offset);
        }
    }

    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        if (shift_pressed) {
            scale_vertices(&model, selection.indices, selection.len, 1.01f);
        } else {
            vec3 offset;
            vec3_set(offset, 0.0f, +0.01f, 0.0f);
            
            move_vertices(&model, selection.indices, selection.len, offset);
        }
    }

    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        if (shift_pressed) {
            scale_vertices(&model, selection.indices, selection.len, 0.99f);
        } else {
            vec3 offset;
            vec3_set(offset, 0.0f, -0.01f, 0.0f);

            move_vertices(&model, selection.indices, selection.len, offset);
        }
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
        handle_selection_move(&selection, normal_x, normal_y);
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    int shift_pressed = (mods & GLFW_MOD_SHIFT);

    int w, h;
    glfwGetWindowSize(window, &w, &h);

    double x, y;
    glfwGetCursorPos(window, &x, &y);
   
    double normal_x, normal_y;
    normalize_mouse_pos(&normal_x, &normal_y, x, y, w, h);
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS)
            handle_selection_start(&selection, normal_x, normal_y);
        else if (action == GLFW_RELEASE)
            handle_selection_end(&selection, normal_x, normal_y, shift_pressed);
    }
}

void set_colour(vec4 colour) {
    vec4_copy(model.palette[0], colour);

    for (int i = 0; i < model.faces_len; i++)
        model.faces[i].color_index = 0;
}
