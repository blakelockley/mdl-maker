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
#include "quad.h"
#include "text.h"
#include "textbox.h"

GLFWwindow *window;

char *filename;
char buffer[128];
int show_fps = 0;

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
    // IMGUI_CHECKVERSION();
    ctx = igCreateContext(NULL);
    io  = igGetIO();

    const char* glsl_version = "#version 330 core";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup style
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

    igBegin("Test", NULL, 0);
    igText("Test");
    igButton("Test",(struct ImVec2){0,0});
    igEnd();

    // // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. 
    // // Here we just want to make the demo initial state a bit more friendly!
    // igSetNextWindowPos((struct ImVec2){0,0}, ImGuiCond_FirstUseEver,(struct ImVec2){0,0} ); 
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
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Engine setup

    srand(time(NULL));

    init_camera(&camera);
    init_light(&light);
    init_grid(&grid);
    init_selection(&selection);
    init_text();
    init_quad();

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
        render_selection(&selection);
        render_model(&model);

        gui_render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free_model(&model);
    free_grid(&grid);
    free_light(&light);
    free_camera(&camera);
    free_selection(&selection);
    free_quad();
    free_text();

    gui_terminate();

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
