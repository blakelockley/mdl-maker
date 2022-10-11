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
#include "viewport.h"
#include "grid.h"
#include "controls.h"
#include "selection.h"
#include "model.h"
#include "light.h"
#include "fps.h"
#include "file.h"
#include "gui.h"
#include "primitives.h"
#include "picker.h"
#include "renderers.h"

#define DEBUG 1

GLFWwindow *window;

selection_t selection;
viewport_t viewport;
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

    // OpenGL setup

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Engine setup

    gui_init(window);

    init_camera(&camera);
    init_light(&light);
    init_grid(&grid);
    init_selection(&selection);
    init_model(&model);

    renderer_t *vertex_renderer = init_vertex_renderer(malloc(sizeof(renderer_t)));
    renderer_t *edge_renderer   = init_edge_renderer(malloc(sizeof(renderer_t)));
    renderer_t *face_renderer   = init_face_renderer(malloc(sizeof(renderer_t)));
    renderer_t *normal_renderer = init_normal_renderer(malloc(sizeof(renderer_t)));

    init_picker(&picker);

    build_icosphere(&model, (vec3){0.0f, 0.5f, 0.0f}, 0.5f, 1);

    while (!glfwWindowShouldClose(window)) {
        #if DEBUG
        sprintf(buffer, "mdl-maker");
        sprintf(buffer, "%s [fps %.2f]", buffer, calculate_fps());
        glfwSetWindowTitle(window, buffer);
        #endif
        
        gui_update();
        update_selection(&selection);
        
        glfwGetFramebufferSize(window, &viewport.width, &viewport.height);

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

        render_model_edges_selection(edge_renderer, &model, selection.indices, selection.len);

        gui_render();

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

    gui_terminate();

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
