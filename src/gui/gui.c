#include "gui.h"

#include <stdlib.h>
#include "glfw.h"
#include "linmath.h"

#include "../camera.h"
#include "../model.h"

#define SHOW_DEMO 1

extern camera_t camera;
extern model_t model;

bool show_add_vertex;

void MainMenuBar();
void AddVertexMenu();

// GUI

void MainMenuBar() {
    if (igBeginMainMenuBar()) {
        
        if (igBeginMenu("File", true)) {
            igMenuItem_Bool("(demo menu)", NULL, false, false);
            
            if (igMenuItem_Bool("New", "Shift+N", false, true)) {
                printf("New Item\n");
            }
            
            if (igMenuItem_Bool("Open", "Ctrl+O", true, false)) {
                printf("Open\n");
            }

            igSeparator();
        
            if (igMenuItem_Bool("Save", "Ctrl+S", false, false)) {
                printf("Save\n");
            }
            
            if (igMenuItem_Bool("Save As..", NULL, false, false)) {
                printf("Save As\n");
            }
            
            igEndMenu();
        }
        
        if (igBeginMenu("View", true))
        {
            if (igMenuItem_Bool("Reset Camera", "", false, true)) {
                init_camera(&camera);
            }
            
            igEndMenu();
        }
        
        if (igBeginMenu("Edit", true))
        {
            if (igMenuItem_Bool("Add Vertex", "Ctrl+A", show_add_vertex, true))
                show_add_vertex = !show_add_vertex;
            
            igEndMenu();
        }
    
        igEndMainMenuBar();
    }

    if (show_add_vertex)
        AddVertexMenu();
}

void AddVertexMenu() {
    static vec3 pos = (vec3){ 0.0f, 0.0f, 0.0f};
    
    if (igBegin("Add Vertex", &show_add_vertex, ImGuiWindowFlags_NoCollapse)) {
        igInputFloat("X", &pos[0], 0.1, 1.0, "%.3f", 0);
        igInputFloat("Y", &pos[1], 0.1, 1.0, "%.3f", 0);
        igInputFloat("Z", &pos[2], 0.1, 1.0, "%.3f", 0);
            
        if (igButton("Add", (struct ImVec2) {0,0})) {
            add_vertex(&model, pos);
            
            vec3_set(pos, 0.0f, 0.0f, 0.0f);
            show_add_vertex = false;
        }

        igEnd();
    }
}

// IMGui

void gui_init(GLFWwindow *window) {
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

    // Custom GUI

    MainMenuBar();

    #if SHOW_DEMO
    igShowDemoWindow(NULL);
    #endif
}

