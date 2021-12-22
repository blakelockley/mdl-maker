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
#include "model.h"
#include "shader.h"
#include "text.h"

GLFWwindow *window;

int width, height;

float zoom = 5.0f;
vec2 scroll_pos = {0.0f, M_PI_4};

int current_index = 1;

model_t object;

void init();
void deinit();
void display_fps();

char buffer[256];

int main() {
    init();

    int shader = load_shader("shaders/vertex.glsl", "shaders/fragment.glsl");

    axis_t axis;
    init_axis(&axis);

    grid_t grid;
    init_grid(&grid);

    init_model(&object);
    add_vertex(&object, (vec3){0.5f, 0.0f, 0.0f});
    add_vertex(&object, (vec3){0.0f, 0.5f, 1.0f});
    add_vertex(&object, (vec3){-0.5f, 0.0f, 0.0f});

    init_text();

    while (!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &width, &height);

        glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        vec3 camera_pos;
        camera_pos[0] = cosf(scroll_pos[1]) * -sinf(scroll_pos[0]) * zoom;
        camera_pos[1] = sinf(scroll_pos[1]) * zoom;
        camera_pos[2] = cosf(scroll_pos[1]) * cosf(scroll_pos[0]) * zoom;

        glUseProgram(shader);

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
        draw_model(&object, current_index, shader);

        display_fps();

        sprintf(buffer, "INDEX:%d\n", current_index);
        render_text(buffer, 0, -32, width, height);

        sprintf(buffer, "ROTATION:%.2f %.2f\n", scroll_pos[0], scroll_pos[1]);
        render_text(buffer, 0, -64, width, height);

        draw_axis(&axis, shader, (float *)scroll_pos, width, height);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free_axis(&axis);
    free_grid(&grid);
    free_model(&object);

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

    if (key == GLFW_KEY_MINUS && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        zoom += 0.5f;
        zoom = fmin(fmax(zoom, 0.5f), 10.0f);
    }

    if (key == GLFW_KEY_EQUAL && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        zoom -= 0.5f;
        zoom = fmin(fmax(zoom, 0.5f), 10.0f);
    }

    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        add_vertex(&object, (vec3){0.0f, 1.0f, 0.0f});
        current_index = object.vertices_len - 1;
    }

    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT))
        move_vertex(&object, current_index, (vec3){0.0f, 0.1f, 0.0f});

    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
        move_vertex(&object, current_index, (vec3){0.0f, -0.1f, 0.0f});

    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
        move_vertex(&object, current_index, (vec3){-0.1f, 0.0f, 0.0f});

    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT))
        move_vertex(&object, current_index, (vec3){0.1f, 0.0f, 0.0f});

    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
        current_index = (current_index + 1) % object.vertices_len;
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

    window = glfwCreateWindow(1200, 800, "mdl-maker", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
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
