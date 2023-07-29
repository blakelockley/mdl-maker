#include <stdlib.h>

#include "selection.h"
#include "renderers.h"

#include "picker.h"
#include "model.h"

extern GLFWwindow *window;
extern struct ImGuiIO* io;

extern model_t model;
extern picker_t picker;

selection_t _selection;
selection_t *selection = &_selection;

void init_selection() {
    selection->indices = (uint32_t*)malloc(sizeof(uint32_t) * 10);
    selection->len = 0;
    selection->cap = 10;

    selection->is_visible = false;
}

void free_selection() {
    free(selection->indices);
}

void start_selection(double mouse_x, double mouse_y) {
    selection->ax = mouse_x;
    selection->ay = mouse_y;

    selection->bx = mouse_x;
    selection->by = mouse_y; 

    selection->is_visible = true;
}

void continue_selection(double mouse_x, double mouse_y) {
    selection->bx = mouse_x;
    selection->by = mouse_y; 
    
    selection->is_visible = true;
}

void finish_selection(double mouse_x, double mouse_y) {
    selection->bx = mouse_x;
    selection->by = mouse_y;

    selection->is_visible = false;

    int shift_pressed = glfwGetKey(GLFW_KEY_LEFT_SHIFT, GLFW_PRESS);

    if (shift_pressed || selection->len == 0) {
        float min_x = fminf(selection->ax, selection->bx);
        float min_y = fminf(selection->ay, selection->by);

        float max_x = fmaxf(selection->ax, selection->bx);
        float max_y = fmaxf(selection->ay, selection->by);

        float width = max_x - min_x;
        float height = max_y - min_y;

        if (width < 5.0f || height < 5.0f)
            return; // selection too small
        
        render_picker_to_vertex_ids(&picker, &model);
        select_ids_in_rect(selection, (vec2){min_x, min_y}, (vec2){max_x, max_y});
    }
}

void update_selection() {
    if (io->WantCaptureMouse)
        return;

    int action = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    
    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);

    if (action == GLFW_PRESS && !selection->is_visible)
        start_selection(mouse_x, mouse_y);
    
    if (action == GLFW_PRESS && selection->is_visible)
        continue_selection(mouse_x, mouse_y);
    
    if (action != GLFW_PRESS && selection->is_visible)
        finish_selection(mouse_x, mouse_y);
}

void render_selection() {
    if (selection->len > 0)
        render_model_vertices_selection(&model, selection->indices, selection->len);
    
    if (selection->is_visible)
        render_selection_box(selection->ax, selection->ay, selection->bx, selection->by, (vec3){0.8f, 0.4f, 0.2f});
}
