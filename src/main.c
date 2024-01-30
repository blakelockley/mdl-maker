#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "glfw.h"
#include "linmath.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ENGINE_INCLUDES
#include "camera.h"
#include "controls.h"
#include "scene/scene.h"
#include "renderers/renderers.h"

#define DEBUG 1

GLFWwindow *window;

struct ImGuiContext* ctx;
struct ImGuiIO* io;

char buffer[128];

void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

int main(int argc, char **argv) {
    char *filename = NULL;
    if (argc >= 2)
        filename = argv[1];
    
    // GLFW Setup
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(1200, 800, "mdl-maker", NULL, NULL);
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

    // ImGui Setup

    ctx = igCreateContext(NULL);
    io  = igGetIO();

    const char* glsl_version = "#version 330 core";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    igStyleColorsDark(NULL);

    // OpenGL setup

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Engine setup

    init_camera();
    init_renderers();

    set_sky_color((vec3){ 0.1f, 0.8f, 1.0f });
    set_ground_color((vec3){ 0.9f, 0.8f, 0.7f });

    // Update-Render loop

    while (!glfwWindowShouldClose(window)) {
        // Update
        
        sprintf(buffer, "mdl-maker - %s", filename);
        glfwSetWindowTitle(window, buffer);
       
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        igNewFrame();

        // Render
        
        glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render_scene();

        igRender();
        ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Terminate ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(ctx);

    // Terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
