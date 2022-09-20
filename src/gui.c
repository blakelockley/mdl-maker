#include "gui.h"

#include <stdlib.h>
#include "glfw.h"
#include "linmath.h"

#include "camera.h"
#include "model.h"

#define SHOW_DEMO 1

extern camera_t camera;
extern model_t model;

bool show_add_vertex;
bool show_add_plane;

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
            if (igMenuItem_Bool("Reset Camera", NULL, false, true)) {
                init_camera(&camera);
            }
            
            igEndMenu();
        }
        
        if (igBeginMenu("Edit", true))
        {
            if (igMenuItem_Bool("Add Vertex", "Ctrl+A", show_add_vertex, true))
                show_add_vertex = !show_add_vertex;
            
            if (igMenuItem_Bool("Add Plane", NULL, show_add_plane, true))
                show_add_plane = !show_add_plane;
            
            igEndMenu();
        }
    
        igEndMainMenuBar();
    }
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

void AddPlaneMenu() {
    static vec3 pos = (vec3){ 0.0f, 0.5f, 0.0f};
    static float side = 0.5f;
    
    if (igBegin("Add Plane", &show_add_plane, ImGuiWindowFlags_NoCollapse)) {
        igText("Position");
        igInputFloat("Position X", &pos[0], 0.1, 1.0, "%.3f", 0);
        igInputFloat("Position Y", &pos[1], 0.1, 1.0, "%.3f", 0);
        igInputFloat("Position Z", &pos[2], 0.1, 1.0, "%.3f", 0);
        
        igText("Side Length");
        igInputFloat("Side Length", &side, 0.1, 1.0, "%.3f", 0);
            
        if (igButton("Add", (struct ImVec2) {0,0})) {
            vec3 vertex;
            uint32_t vertices[4];
            
            vec3_add(vertex, pos, (vec3){side, 0.0f, side});
            vertices[0] = add_vertex(&model, vertex);
            
            vec3_add(vertex, pos, (vec3){side, 0.0f, -side});
            vertices[1] = add_vertex(&model, vertex);
            
            vec3_add(vertex, pos, (vec3){-side, 0.0f, -side});
            vertices[2] = add_vertex(&model, vertex);
            
            vec3_add(vertex, pos, (vec3){-side, 0.0f, side});
            vertices[3] = add_vertex(&model, vertex);

            add_face(&model, vertices, 4);
            
            vec3_set(pos, 0.0f, 0.0f, 0.0f);
            side = 0.5f;
            
            show_add_plane = false;
        }

        igEnd();
    }
}

void DebugWindow() {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration 
        | ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoFocusOnAppearing
        | ImGuiWindowFlags_NoNav 
        | ImGuiWindowFlags_NoMove;

    igSetNextWindowBgAlpha(0.35f);
    if (igBegin("Camera", NULL, flags)) {
        igText("camera.pos %.2f, %.2f, %.2f", camera.pos[0], camera.pos[1], camera.pos[2]);
        igText("camera.dir %.2f, %.2f, %.2f", camera.dir[0], camera.dir[1], camera.dir[2]);
        igText("camera.len %f", vec3_len(camera.pos));
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
    DebugWindow();

    if (show_add_vertex)
        AddVertexMenu();

    if (show_add_plane)
        AddPlaneMenu();

    #if SHOW_DEMO
    igShowDemoWindow(NULL);
    #endif
}

