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
#include "controls.h"
#include "grid.h"
#include "model.h"
#include "shader.h"
#include "stage.h"
#include "text.h"

GLFWwindow *window;
int width, height;

extern int selection_len;
extern camera_t camera;
model_t object;

void display_fps();

char buffer[256];

void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

int main() {
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

    int shader = load_shader("shaders/vertex.glsl", "shaders/fragment.glsl");

    init_axis();
    init_camera();
    init_grid();

    init_model(&object);
    add_vertex(&object, (vec3){0.0f, 0.5f, 0.0f});

    init_text();

    while (!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &width, &height);

        glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw_axis();
        draw_grid(shader);
        draw_model(&object, shader);

        display_fps();

        sprintf(buffer, "SELECTED:%d\n", selection_len);
        render_text(buffer, 0, -32, width, height);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free_axis();
    free_model(&object);
    free_grid();
    free_text();

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
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
