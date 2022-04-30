#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "glfw.h"
#include "linmath.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ENGINE_INCLUDES
#include "camera.h"
#include "viewport.h"
#include "grid.h"
#include "controls.h"
#include "selection.h"
#include "model.h"
#include "face_renderer.h"
#include "normal_renderer.h"
#include "light.h"

GLFWwindow *window;

char *filename;

selection_t selection;
viewport_t viewport;
camera_t camera;
light_t light;
grid_t grid;
model_t model;

void display_fps();

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
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // OpenGL setup

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    init_camera(&camera);
    init_light(&light);
    init_grid(&grid);
    init_selection(&selection);
    init_model(&model);
    
    init_face_renderer();
    init_normal_renderer();
    
    add_vertex(&model, (vec3){0.25, 0.5, 0.0});
    add_vertex(&model, (vec3){-0.25, 0.5, 0.0});
    add_vertex(&model, (vec3){0.0, 0.35, 0.25});

    while (!glfwWindowShouldClose(window)) {
        display_fps();
        
        glfwGetFramebufferSize(window, &viewport.width, &viewport.height);

        glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render_grid(&grid);
        render_selection(&selection);
        render_model(&model);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free_model(&model);
    free_grid(&grid);
    free_light(&light);
    free_camera(&camera);
    free_selection(&selection);

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

void display_fps() {
    static char fps_text[32];
    static double last_second = 0;
    static int frames = 0;

    double current_time = glfwGetTime();

    frames++;
    if (current_time - last_second > 1.0) {
        double fps = frames / (current_time - last_second);

        sprintf(fps_text, "mdl-maker (FPS: %.2f)", fps);
        glfwSetWindowTitle(window, fps_text);

        frames = 0;
        last_second = current_time;
    }
}
