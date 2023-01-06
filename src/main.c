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
#include "grid.h"
#include "controls.h"
#include "selection.h"
#include "model.h"
#include "light.h"
#include "fps.h"
#include "file.h"
#include "menu.h"
#include "primitives.h"
#include "picker.h"
#include "renderers.h"

#define DEBUG 1

GLFWwindow *window;

struct ImGuiContext* ctx;
struct ImGuiIO* io;

selection_t selection;
camera_t camera;
light_t light;
grid_t grid;
model_t model;
picker_t picker;

char buffer[128];

extern bool render_vertices;
extern bool render_edges;
extern bool render_faces;
extern bool render_normals;

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

    init_camera(&camera);
    init_light(&light);
    init_grid(&grid);
    init_selection(&selection);
    init_model(&model);

    init_debug_renderer();
    renderer_t *vertex_renderer = init_vertex_renderer(malloc(sizeof(renderer_t)));
    renderer_t *edge_renderer   = init_edge_renderer(malloc(sizeof(renderer_t)));
    renderer_t *face_renderer   = init_face_renderer(malloc(sizeof(renderer_t)));
    renderer_t *normal_renderer = init_normal_renderer(malloc(sizeof(renderer_t)));

    init_picker(&picker);

    if (filename)
        open_file(filename, &model);

    // Update-Render loop

    while (!glfwWindowShouldClose(window)) {
        sprintf(buffer, "mdl-maker - %s", model.filename);
        glfwSetWindowTitle(window, buffer);
       
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        igNewFrame();

        update_menu();
        update_selection(&selection);
        debug_camera(&camera);

        glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render_grid(&grid);

        if (render_vertices)
            vertex_renderer->render(vertex_renderer, &model);
        
        if (render_edges)
            edge_renderer->render(edge_renderer, &model);
        
        if (render_faces)
            face_renderer->render(face_renderer, &model);
        
        if (render_normals)
            normal_renderer->render(normal_renderer, &model);

        render_selection(&selection, vertex_renderer, edge_renderer);
        render_debug_shapes();

        igRender();
        ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free_picker(&picker);

    deinit_renderer(vertex_renderer);
    free(vertex_renderer);

    deinit_renderer(edge_renderer);
    free(edge_renderer);

    deinit_renderer(face_renderer);
    free(face_renderer);

    deinit_renderer(normal_renderer);
    free(normal_renderer);

    free_model(&model);
    free_grid(&grid);
    free_light(&light);
    free_camera(&camera);
    free_selection(&selection);

    // Terminate ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(ctx);

    // Terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
