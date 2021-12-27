#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "glfw.h"
#include "linmath.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ENGINE_INCLUDES
#include "axis.h"
#include "camera.h"
#include "grid.h"
#include "model.h"
#include "shader.h"
#include "text.h"

GLFWwindow *window;

int width, height;
int third_person = 0;
int shift_pressed = 0;

int selection_len = 0;
int selection_buffer[256];

camera_t camera;
model_t object;

void init();
void deinit();
void display_fps();

unsigned int current_command = '\0';
char buffer[256];

int main() {
    init();

    int shader = load_shader("shaders/vertex.glsl", "shaders/fragment.glsl");

    init_axis();

    init_model(&object);
    add_vertex(&object, (vec3){0.0f, 0.0f, 0.0f});

    init_text();

    while (!glfwWindowShouldClose(window)) {
        vec3 camera_pos = (vec3){5.0f, 5.0f, 5.0f};
        if (!third_person)
            vec3_copy(camera_pos, camera.pos);

        glfwGetFramebufferSize(window, &width, &height);

        glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw_axis();
        draw_model(&object, camera_pos, shader);

        display_fps();

        sprintf(buffer, "SELECTED:%d\n", selection_len);
        render_text(buffer, 0, -32, width, height);

        sprintf(buffer, "COMMAND:%c\n", current_command);
        render_text(buffer, 0, -64, width, height);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free_axis();
    free_model(&object);
    free_camera(&camera);

    free_text();

    deinit();
    return EXIT_SUCCESS;
}

void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if ((key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE) && action == GLFW_PRESS) {
        current_command = '\0';
        selection_len = 0;
    }

    if (key == GLFW_KEY_MINUS && (action == GLFW_PRESS || action == GLFW_REPEAT))
        update_zoom(&camera, 0.5f);

    if (key == GLFW_KEY_EQUAL && (action == GLFW_PRESS || action == GLFW_REPEAT))
        update_zoom(&camera, -0.5f);

    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        add_vertex(&object, (vec3){0.0f, 1.0f, 0.0f});

        selection_len = 0;
        selection_buffer[selection_len++] = object.vertices_len - 1;
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        add_face(&object);

    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        vec3 vector;
        vec3_copy(vector, camera.right);
        vec3_scale(vector, vector, -0.1f);

        move_selection(&object, vector);
    }

    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        vec3 vector;
        vec3_copy(vector, camera.right);
        vec3_scale(vector, vector, 0.1f);

        move_selection(&object, vector);
    }

    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
        move_selection(&object, (vec3){0.0f, -0.1f, 0.0f});

    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT))
        move_selection(&object, (vec3){0.0f, 0.1f, 0.0f});

    if (key == GLFW_KEY_0 && action == GLFW_PRESS)
        third_person = !third_person;

    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
        shift_pressed = 1;

    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
        shift_pressed = 0;
}

void character_callback(GLFWwindow *window, unsigned int codepoint) {
    if ('a' <= codepoint && codepoint <= 'z')
        current_command = codepoint - 32;
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        int w, h;
        glfwGetWindowSize(window, &w, &h);

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        set_ray(&camera, xpos, ypos, w, h);
        int index = find_intercept(&object, &camera);

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
    if (current_command == 'X')
        return move_selection(&object, (vec3){xoffset, 0.0f, 0.0f});

    if (current_command == 'Y')
        return move_selection(&object, (vec3){0.0f, yoffset, 0.0f});

    if (current_command == 'Z')
        return move_selection(&object, (vec3){0.0f, 0.0f, xoffset});

    if (shift_pressed)
        yoffset = 0.0;

    update_scroll(&camera, xoffset, yoffset);
}

void init() {
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "mdl-maker", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCharCallback(window, character_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // OpenGL setup

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void deinit() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void display_fps() {
    static char fps_text[16];
    static double last_second = 0;
    static int frames = 0;

    double current_time = glfwGetTime();

    frames++;
    if (current_time - last_second > 1.0) {
        double fps = frames / (current_time - last_second);
        sprintf(fps_text, "FPS:%.2f", fps);

        frames = 0;
        last_second = current_time;
    }

    render_text(fps_text, 0, 0, width, height);
}
