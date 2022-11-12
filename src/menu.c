#include "menu.h"

#include <stdlib.h>
#include "glfw.h"
#include "linmath.h"

#include "camera.h"
#include "model.h"
#include "selection.h"
#include "primitives.h"
#include "macros.h"

#define SHOW_DEMO 1

extern camera_t camera;
extern model_t model;
extern selection_t selection;

bool render_vertices = true;
bool render_edges = false;
bool render_faces = true;
bool render_normals = false;

void AddVertexMenu(bool *p_open);
void AddPlaneMenu(bool *p_open);
void AddDiscMenu(bool *p_open);
void AddIcosphereMenu(bool *p_open);

// ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration 
//     | ImGuiWindowFlags_AlwaysAutoResize
//     | ImGuiWindowFlags_NoSavedSettings
//     | ImGuiWindowFlags_NoFocusOnAppearing
//     | ImGuiWindowFlags_NoNav 
//     | ImGuiWindowFlags_NoMove;

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

            igSeparator();
            igMenuItem_Bool("Renderers", NULL, false, false);
            
            if (igMenuItem_Bool("Render Vertices", NULL, render_vertices, true))
                render_vertices = !render_vertices;
            
            if (igMenuItem_Bool("Render Edges", NULL, render_edges, true))
                render_edges = !render_edges;
            
            if (igMenuItem_Bool("Render Faces", NULL, render_faces, true))
                render_faces = !render_faces;
            
            if (igMenuItem_Bool("Render Normals", NULL, render_normals, true))
                render_normals = !render_normals;
            
            igEndMenu();
        }
        
        static bool show_add_vertex = false;
        static bool show_add_plane = false;
        static bool show_add_disc = false;
        static bool show_add_icosphere = false;
        
        if (igBeginMenu("Edit", true))
        {
            if (igMenuItem_Bool("Add Vertex", "Ctrl+A", show_add_vertex, true))
                show_add_vertex = !show_add_vertex;
     
            if (igMenuItem_Bool("Add Plane", NULL, show_add_plane, true))
                show_add_plane = !show_add_plane;
            
            if (igMenuItem_Bool("Add Disc", NULL, show_add_disc, true))
                show_add_disc = !show_add_disc;
            
            if (igMenuItem_Bool("Add Icosphere", NULL, show_add_icosphere, true))
                show_add_icosphere = !show_add_icosphere;
            
            igEndMenu();
        }

        if (show_add_vertex)
            AddVertexMenu(&show_add_vertex);

        if (show_add_plane)
            AddPlaneMenu(&show_add_plane);

        if (show_add_disc)
            AddDiscMenu(&show_add_disc);

        if (show_add_icosphere)
            AddIcosphereMenu(&show_add_icosphere);

        if (igBeginMenu(selection.mode == MODE_VERTEX ? "Select: Vertices" : "Select: Faces", true))
        {
            if (igMenuItem_Bool("Select Vertices", NULL, selection.mode == MODE_VERTEX, true)) {
                selection.len = 0;
                selection.mode = MODE_VERTEX;
            }
     
            if (igMenuItem_Bool("Select Faces", NULL, selection.mode == MODE_FACE, true)) {
                selection.len = 0;
                selection.mode = MODE_FACE;
            }
            
            igEndMenu();
        }

        #ifdef SHOW_DEMO
        
        static bool show_demo_window = false;
        
        if (igBeginMenu("ImGui", true))
        {
            if (igMenuItem_Bool("Show ImGui Demo Window", NULL, show_demo_window, true))
                show_demo_window = !show_demo_window;
            igEndMenu();
        }
        
        // https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp
        if (show_demo_window)
            igShowDemoWindow(&show_demo_window);
       
        #endif
        
        igEndMainMenuBar();
    }
}

void AddVertexMenu(bool *p_open) {
    static vec3 pos = (vec3){ 0.0f, 0.0f, 0.0f};
    
    if (igBegin("Add Vertex", p_open, ImGuiWindowFlags_NoCollapse)) {
        igInputFloat("X", &pos[0], 0.1, 1.0, "%.3f", 0);
        igInputFloat("Y", &pos[1], 0.1, 1.0, "%.3f", 0);
        igInputFloat("Z", &pos[2], 0.1, 1.0, "%.3f", 0);
            
        if (igButton("Add", (struct ImVec2) {0,0})) {
            add_vertex(&model, pos);
            
            vec3_set(pos, 0.0f, 0.0f, 0.0f);
            *p_open = false;
        }

        igEnd();
    }
}

void AddPlaneMenu(bool *p_open) {
    static vec3 pos = (vec3){ 0.0f, 0.5f, 0.0f};
    static float side = 0.5f;
    static int n_subdivisions = 0;
    
    if (igBegin("Add Plane", p_open, ImGuiWindowFlags_NoCollapse)) {
        igText("Position");
        igInputFloat("Position X", &pos[0], 0.1, 1.0, "%.3f", 0);
        igInputFloat("Position Y", &pos[1], 0.1, 1.0, "%.3f", 0);
        igInputFloat("Position Z", &pos[2], 0.1, 1.0, "%.3f", 0);
        
        igText("Side Length");
        igInputFloat("Side Length", &side, 0.1, 1.0, "%.3f", 0);
        
        igText("Subdivisions");
        igInputInt("Subdivisions", &n_subdivisions, 1, 2, 0);
        n_subdivisions = MAX(n_subdivisions, 0);
            
        if (igButton("Add", (struct ImVec2) {0,0})) {
            build_plane(&model, pos, side, n_subdivisions);
            *p_open = false;
        }

        igEnd();
    }
}

void AddDiscMenu(bool *p_open) {
    static vec3 pos = (vec3){ 0.0f, 0.5f, 0.0f};
    static float radius = 0.5f;
    static int n_vertices = 12;
    
    if (igBegin("Add Disc", p_open, ImGuiWindowFlags_NoCollapse)) {
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

            *p_open = false;
        }

        igEnd();
    }
}

void AddIcosphereMenu(bool *p_open) {
    static vec3 pos = (vec3){ 0.0f, 0.5f, 0.0f};
    static float radius = 0.5f;
    static int order = 0;
    
    if (igBegin("Add Icosphere", p_open, ImGuiWindowFlags_NoCollapse)) {
        igText("Position");
        igInputFloat3("Position X", pos, "%.2f", 0);
        
        igText("Radius");
        igInputFloat("Radius", &radius, 0.1, 1.0, "%.3f", 0);
        
        igText("Num. of Recusions");
        igInputInt("Num. of Recusions", &order, 1, 2, 0);
            
        if (igButton("Add", (struct ImVec2) {0,0})) {
            build_icosphere(&model, pos, radius, order);
            
            // reset menu
            vec3_set(pos, 0.0f, 0.0f, 0.0f);
            radius = 0.5f;
            order = 0.0f;
            *p_open = false;
        }

        igEnd();
    }
}

void update_menu() {
    MainMenuBar();   
}