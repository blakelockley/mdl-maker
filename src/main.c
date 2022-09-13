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
#include "callbacks.h"
#include "selection.h"
#include "model.h"
#include "light.h"
#include "fps.h"
#include "file.h"

GLFWwindow *window;

char *filename;
char buffer[128];
int show_fps = 0;

bool is_open = 1;
bool show_circle_menu = 0;

selection_t selection;
viewport_t viewport;
camera_t camera;
light_t light;
grid_t grid;
model_t model;

struct ImGuiContext* ctx;
struct ImGuiIO* io;

void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

void gui_init() {
    ctx = igCreateContext(NULL);
    io  = igGetIO();

    const char* glsl_version = "#version 330 core";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    igStyleColorsDark(NULL);
}

void gui_terminate() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(ctx);
}

void gui_render() {
    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}

void gui_update() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();

    if (is_open) {
        igBegin("Object Colour", &is_open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        if (igButton("Reset", (struct ImVec2){0,0}))
            set_colour((vec3){1.0f, 1.0f, 1.0f});

        // // Edit a color (stored as ~4 floats)
        igColorEdit4("Color 1", model.palette[0], 0);
        igColorEdit4("Color 2", model.palette[1], 0);
        igEnd();
    }


    if (show_circle_menu) {
        igBegin("Add Circle", &show_circle_menu, ImGuiWindowFlags_NoCollapse);
        
        static int vertices = 8;
        static float radius = 0.1f;

        igInputInt("Vertices", &vertices, 1, 1, 0);
        igInputFloat("Radius", &radius, 0.1, 1.0, "%.3f", 0);
        
        if (igButton("Add", (struct ImVec2){0,0})) {
            add_circle(vertices, radius);
            show_circle_menu = 0;
        }

        igEnd();
    }

    igShowDemoWindow(NULL);
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

    srand(time(NULL));

    init_camera(&camera);
    init_light(&light);
    init_grid(&grid);
    init_selection(&selection);
    gui_init();
    
    init_model(&model);
    open_file(filename, &model);

    while (!glfwWindowShouldClose(window)) {
        sprintf(buffer, "mdl-maker - %s", filename);
        if (show_fps)
            sprintf(buffer, "%s [fps %.2f]", buffer, calculate_fps());

        glfwSetWindowTitle(window, buffer);
        
        gui_update();
        
        glfwGetFramebufferSize(window, &viewport.width, &viewport.height);

        glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        render_grid(&grid);
        render_model(&model);

        gui_render();

        if (!io->WantCaptureMouse)
            render_selection(&selection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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
