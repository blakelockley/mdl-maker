#include "gui.h"

#include <stdlib.h>
#include "glfw.h"
#include "linmath.h"

#include "camera.h"
#include "model.h"
#include "selection.h"

#define SHOW_DEMO 1

extern camera_t camera;
extern model_t model;
extern selection_t selection;

bool show_add_vertex;
bool show_add_plane;
bool show_add_disc;

void set_mode_axis(uint8_t axis) {
    mode_axis = axis;

    switch (mode_axis){
    case MODE_AXIS_X:
        vec3_set(selection.control_axis, 1.0f, 0.0f, 0.0f);
        break;
    
    case MODE_AXIS_Y:
        vec3_set(selection.control_axis, 0.0f, 1.0f, 0.0f);
        break;
    
    case MODE_AXIS_Z:
        vec3_set(selection.control_axis, 0.0f, 0.0f, 1.0f);
        break;
    
    default:
        vec3_set(selection.control_axis, 0.0f, 0.0f, 0.0f);
        break;
    }

    update_control_axis(&selection);
}

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
            
            if (igMenuItem_Bool("Add Disc", NULL, show_add_disc, true))
                show_add_disc = !show_add_disc;
            
            igEndMenu();
        }
        
        if (igBeginMenu("Mode", true))
        {
            if (igMenuItem_Bool("Select", NULL, !mode, true))
                mode = MODE_SELECT;
            
            if (igMenuItem_Bool("Translate", NULL, mode & MODE_TRANSLATE, true))
                mode = MODE_TRANSLATE;
            
            if (igMenuItem_Bool("Rotate", NULL, mode & MODE_ROTATE, true))
                mode = MODE_ROTATE;
            
            if (igMenuItem_Bool("Scale", NULL, mode & MODE_SCALE, true))
                mode = MODE_SCALE;

            igSeparator();

            igMenuItem_Bool("Confine to axis", NULL, false, false);

            if (igMenuItem_Bool("None", NULL, !mode_axis, true))
                set_mode_axis(MODE_AXIS_NONE);
            
            if (igMenuItem_Bool("X Axis", NULL, mode_axis & MODE_AXIS_X, true))
                set_mode_axis(MODE_AXIS_X);
            
            if (igMenuItem_Bool("Y Axis", NULL, mode_axis & MODE_AXIS_Y, true))
                set_mode_axis(MODE_AXIS_Y);
            
            if (igMenuItem_Bool("Z Axis", NULL, mode_axis & MODE_AXIS_Z, true))
                set_mode_axis(MODE_AXIS_Z);
            
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

void AddDiscMenu() {
    static vec3 pos = (vec3){ 0.0f, 0.5f, 0.0f};
    static float radius = 0.5f;
    static int n_vertices = 12;
    
    if (igBegin("Add Disc", &show_add_disc, ImGuiWindowFlags_NoCollapse)) {
        igText("Position");
        igInputFloat("Position X", &pos[0], 0.1, 1.0, "%.3f", 0);
        igInputFloat("Position Y", &pos[1], 0.1, 1.0, "%.3f", 0);
        igInputFloat("Position Z", &pos[2], 0.1, 1.0, "%.3f", 0);
        
        igText("Radius");
        igInputFloat("Radius", &radius, 0.1, 1.0, "%.3f", 0);
        
        igText("Num. Vertices");
        igInputInt("Num. Vertices", &n_vertices, 1, 2, 0);
            
        if (igButton("Add", (struct ImVec2) {0,0})) {
            float step = (M_PI * 2) / n_vertices;

            // center
            uint32_t center = add_vertex(&model, pos);

            // edge vertices
            uint32_t vertices[n_vertices];
            
            for (int i = 0; i < n_vertices; i++) {
                vec3 v;
                vec3_add(v, pos, (vec3){cosf(step * i) * radius, 0.0f, sinf(step * i) * radius});

                vertices[i] = add_vertex(&model, v);
            }

            uint32_t indices[3];
            
            // face sectors
            for (int i = 0; i < n_vertices; i++) {
                indices[0] = center;
                indices[1] = vertices[i];
                indices[2] = vertices[(i + 1) % n_vertices];
                add_face(&model, indices, 3);
            }

            show_add_disc = false;
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

    if (show_add_disc)
        AddDiscMenu();

    #if SHOW_DEMO
    igShowDemoWindow(NULL);
    #endif
}

