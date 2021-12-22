#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "glfw.h"
#include "linmath.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ENGINE_INCLUDES
#include "axis.h"
#include "grid.h"
#include "shader.h"
#include "text.h"

GLFWwindow *window;

vec2 scroll_pos = {0.0f, M_PI_4};

int zoom_dir = 0;
float zoom = 5.0f;

void init();
void deinit();

int main() {
    init();

    char fps_text[16] = {};

    double time_elapsed = 0, last_second = 0;
    int frames = 0;

    int shader = load_shader("shaders/vertex.glsl", "shaders/fragment.glsl");

    axis_t axis;
    init_axis(&axis);

    grid_t grid;
    init_grid(&grid);

    init_text();

    while (!glfwWindowShouldClose(window)) {
        double current_time = glfwGetTime();
        double delta = current_time - time_elapsed;
        time_elapsed = current_time;

        frames++;
        if (current_time - last_second > 1.0) {
            double fps = frames / (current_time - last_second);
            sprintf(fps_text, "FPS:%.2f", fps);

            frames = 0;
            last_second = current_time;
        }

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader);

        zoom += delta * zoom_dir * 2.5f;
        zoom = fmin(fmax(zoom, 0.5f), 10.0f);

        vec3 camera_pos;
        camera_pos[0] = sinf(scroll_pos[1]) * cosf(scroll_pos[0]) * zoom;
        camera_pos[1] = cosf(scroll_pos[1]) * zoom;
        camera_pos[2] = sinf(scroll_pos[1]) * sinf(scroll_pos[0]) * zoom;

        mat4x4 model, view, projection;
        mat4x4_identity(model);
        mat4x4_look_at(view, camera_pos, (vec3){0, 0, 0}, (vec3){0, 1, 0});
        mat4x4_perspective(projection, 45.0f, (float)width / (float)height, 0.1f, 100.0f);

        GLint model_loc = glGetUniformLocation(shader, "model");
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float *)model);

        GLint view_loc = glGetUniformLocation(shader, "view");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float *)view);

        GLint projection_loc = glGetUniformLocation(shader, "projection");
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float *)projection);

        // Render...
        draw_grid(&grid, shader);
        draw_axis(&axis, shader, (float *)scroll_pos, width, height);

        render_text(fps_text, 0, 0, width, height);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free_axis(&axis);
    free_grid(&grid);

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

    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        zoom_dir = -1;

    if (key == GLFW_KEY_UP && action == GLFW_RELEASE)
        zoom_dir = 0;

    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        zoom_dir = +1;

    if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
        zoom_dir = 0;
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    // pass
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    scroll_pos[0] += xoffset;
}

void init() {
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "GLFW Window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetWindowTitle(window, "mdl-maker");

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // OpenGL setup

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void deinit() {
    glfwDestroyWindow(window);
    glfwTerminate();
}
