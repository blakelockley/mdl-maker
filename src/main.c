#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "glfw.h"
#include "linmath.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ENGINE_INCLUDES
#include "axis.h"
#include "bounds.h"
#include "camera.h"
#include "controls.h"
#include "filemanager.h"
#include "light.h"
#include "object.h"
#include "stage.h"
#include "text.h"

GLFWwindow *window;
int width, height;

char *filename;

extern int selection_len;
extern camera_t camera;
object_t object;

void display_fps();

char buffer[256];

void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    filename = argv[1];

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

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    init_axis();
    init_camera();
    init_light();
    init_bounds();

    init_object(&object);
    open_file(filename, &object);
    buffer_object(&object);

    init_text();

    while (!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &width, &height);

        glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw_axis();
        draw_light();
        draw_bounds();
        draw_object(&object);

        display_fps();

        sprintf(buffer, "SELECTED:%d\n", selection_len);
        render_text(buffer, 0, -32, width, height);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free_object(&object);

    free_axis();
    free_light();
    free_bounds();
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
