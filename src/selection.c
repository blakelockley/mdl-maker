#include "selection.h"

#include <stdio.h>
#include <stdlib.h>

#include "camera.h"
#include "shader.h"
#include "model.h"
#include "picker.h"

#include "renderers.h"

#define HANDLE_SIZE 16

// TODO: Update extend code and add co-planar calculations
// TODO: Have resize handle positioned at bottom-right

extern camera_t camera;
extern model_t model;
extern picker_t picker;

extern GLFWwindow *window;

static selection_t _selection;
selection_t *selection = &_selection;

// update methods

void update_selection_faces();
void update_selection_midpoint();
void update_selection_menu();
void update_selection_box();

// selection

void start_selection(double mouse_x, double mouse_y);
void continue_selection(double mouse_x, double mouse_y);
void finish_selection(double mouse_x, double mouse_y);

// moving

void start_moving(double mouse_x, double mouse_y);
void continue_moving(double mouse_x, double mouse_y);
void finish_moving(double mouse_x, double mouse_y);

// resizing

void start_resizing(double mouse_x, double mouse_y);
void continue_resizing(double mouse_x, double mouse_y);
void finish_resizing(double mouse_x, double mouse_y);

// utilities

bool point_inside_selection(double x, double y);
bool point_inside_handle(double x, double y);

void init_selection(renderer_t *selection_renderer, renderer_t *control_renderer, renderer_t *vertex_renderer, renderer_t *edge_renderer) {
    selection->indices = (uint32_t*)malloc(sizeof(uint32_t) * 10);
    selection->len = 0;
    selection->cap = 10;

    selection->faces = (uint32_t*)malloc(sizeof(uint32_t) * 10);
    selection->faces_len = 0;
    selection->faces_cap = 10;

    selection->selection_renderer = selection_renderer;
    selection->control_renderer = control_renderer;
    selection->vertex_renderer = vertex_renderer;
    selection->edge_renderer = edge_renderer;

    selection->state = INITIAL;
    
    selection->is_hovering = false;
    selection->is_hovering_handle = false;
}

void free_selection() {
    free(selection->indices);
    free(selection->faces);
}

void update_selection() {
    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
   
    int action = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    
    bool is_inside = point_inside_selection(mouse_x, mouse_y);
    bool is_inside_handle = point_inside_handle(mouse_x, mouse_y);

    switch (selection->state) {
    case INITIAL:
        if (action == GLFW_PRESS)
            start_selection(mouse_x, mouse_y);
        
        break;
        
    case SELECTING:
        if (action == GLFW_PRESS)
            continue_selection(mouse_x, mouse_y);

        if (action == GLFW_RELEASE)
            finish_selection(mouse_x, mouse_y);
        
        break;
        
    case SELECTED:
        update_selection_box();
        update_selection_midpoint();
        
        selection->is_hovering = is_inside;
        selection->is_hovering_handle = selection->len > 1 && is_inside_handle;

        if (action == GLFW_PRESS) {
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
                start_selection(mouse_x, mouse_y);
            
            else if (is_inside_handle)
                start_resizing(mouse_x, mouse_y);
            
            else if (is_inside)
                start_moving(mouse_x, mouse_y);
        }
        
        break;

    case MOVING:
        update_selection_box();
        update_selection_midpoint();
        
        if (action == GLFW_PRESS)
            continue_moving(mouse_x, mouse_y);

        if (action == GLFW_RELEASE)
            finish_moving(mouse_x, mouse_y);
        
        break;

    case RESIZING:
        update_selection_box();
        update_selection_midpoint();
        
        if (action == GLFW_PRESS)
            continue_resizing(mouse_x, mouse_y);

        if (action == GLFW_RELEASE)
            finish_resizing(mouse_x, mouse_y);
        
        break;
    
    default:
        break;
    }

    if (selection->len > 0)
        update_selection_menu();
}

void start_selection(double mouse_x, double mouse_y) {
    selection->ax = mouse_x;
    selection->ay = mouse_y;

    selection->bx = mouse_x;
    selection->by = mouse_y; 

    selection->state = SELECTING;
}

void continue_selection(double mouse_x, double mouse_y) {
    selection->bx = mouse_x;
    selection->by = mouse_y; 
    
    selection->state = SELECTING;
}

void finish_selection(double mouse_x, double mouse_y) {
    selection->bx = mouse_x;
    selection->by = mouse_y;
    
    int shift_pressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
    if (!shift_pressed)
        clear_selection(selection);

    if (selection->ax == selection->bx && selection->ay == selection->by) {
        uint32_t picked_face_index = render_picker_to_face_id(&picker, &model);
        if (picked_face_index != INDEX_NOT_FOUND) {
            
            face_t *face = &model.faces[picked_face_index];
            for (int i = 0; i < face->len; i++) {
                uint32_t index = face->indices[i];
                
                // check if index already exists in selection
                bool already_exists = false;
                for (int j = 0; j < selection->len && !already_exists; j++)
                    already_exists = (selection->indices[j] == index);

                if (already_exists)
                    continue;
                
                if (selection->len == selection->cap) {
                    selection->cap *= 2;
                    selection->indices = (uint32_t*)realloc(selection->indices, sizeof(uint32_t) * selection->cap);
                }

                selection->indices[selection->len++] = index;
            }
        }
    
    } else if (shift_pressed || selection->len == 0) {
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

    if (selection->len > 0)
        selection->state = SELECTED;
    else
        selection->state = INITIAL;
}

void start_moving(double mouse_x, double mouse_y) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    float clip_x = (mouse_x / width) * 2.0f - 1.0f;
    float clip_y = 1.0f - (mouse_y / height) * 2.0f;
    
    mat4x4 view, projection, vp, inverse_vp;
    get_view_matrix(&camera, view);
    get_projection_matrix(&camera, projection);
    
    mat4x4_mul(vp, projection, view);

    vec4 midpoint, projected_midpoint;
    vec4_from_vec3(midpoint, selection->midpoint, 1);
    mat4x4_mul_vec4(projected_midpoint, vp, midpoint);
    
    float clip_z = projected_midpoint[2] / projected_midpoint[3]; // perspective divide
    
    vec4 v, r;
    vec4_set(v, clip_x, clip_y, clip_z, 1);
    
    mat4x4_invert(inverse_vp, vp);
    mat4x4_mul_vec4(r, inverse_vp, v);
    
    vec3 plane_pos;
    vec3_from_vec4(plane_pos, r);

    vec3_sub(selection->offset, plane_pos, selection->midpoint);

    selection->state = MOVING;
}

void continue_moving(double mouse_x, double mouse_y) {
    mat4x4 view, projection, vp, inverse_vp;
    get_view_matrix(&camera, view);
    get_projection_matrix(&camera, projection);
    
    mat4x4_mul(vp, projection, view);

    vec4 midpoint, projected_midpoint;
    vec4_from_vec3(midpoint, selection->midpoint, 1);
    
    mat4x4_mul_vec4(projected_midpoint, vp, midpoint);
    vec4_scale(projected_midpoint, projected_midpoint, 1 / projected_midpoint[3]); // perspective divide

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    float clip_x = (mouse_x / width) * 2.0f - 1.0f;
    float clip_y = 1.0f - (mouse_y / height) * 2.0f;
    float clip_z = projected_midpoint[2];
    
    vec4 v, r;
    vec4_set(v, clip_x, clip_y, clip_z, 1);
    
    mat4x4_invert(inverse_vp, vp);
    mat4x4_mul_vec4(r, inverse_vp, v);
    
    vec3 plane_pos;
    vec3_from_vec4(plane_pos, r);
    vec3_sub(plane_pos, plane_pos, selection->offset);

    for (int i = 0; i < selection->len; i++) {
        uint32_t index = selection->indices[i];

        vec3 delta;
        vec3_sub(delta, model.vertices[index], selection->midpoint);
        
        vec3_add(model.vertices[index], plane_pos, delta);
    }

    update_selection_midpoint();
}

void finish_moving(double mouse_x, double mouse_y) {
    selection->state = SELECTED;
}

void start_resizing(double mouse_x, double mouse_y) {
    selection->resize_x = mouse_x;
    selection->resize_y = mouse_y;

    selection->state = RESIZING;
}

void continue_resizing(double mouse_x, double mouse_y) {
    float delta_x = mouse_x - selection->resize_x;
    float delta_y = (mouse_y - selection->resize_y) * -1.0f;
    
    selection->resize_x = mouse_x;
    selection->resize_y = mouse_y;

    float delta = delta_x;
    if (fabs(delta_y) > fabs(delta_x))
        delta = delta_y;

    float factor = 0.01f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        factor = 0.001f;

    float scale = 1 + delta * factor;
    
    for (int i = 0; i < selection->len; i++) {
        uint32_t index = selection->indices[i];
        
        vec3 delta;
        vec3_sub(delta, model.vertices[index], selection->midpoint);
        vec3_scale(delta, delta, scale);
        
        vec3_add(model.vertices[selection->indices[i]], selection->midpoint, delta);
    }
}

void finish_resizing(double mouse_x, double mouse_y) {
    selection->state = SELECTED;
}

void render_selection() {
    if (selection->state == SELECTING)
        render_selection_box(selection->selection_renderer, selection->ax, selection->ay, selection->bx, selection->by, (vec3){0.8f, 0.4f, 0.2f});

    if (selection->state == SELECTED || selection->state == MOVING || selection->state == RESIZING) {
        vec3 color;
        if (selection->is_hovering && !selection->is_hovering_handle)
            vec3_set(color, 1.0f, 1.0f, 1.0f);
        else
            vec3_set(color, 0.5f, 0.5f, 0.5f);
        
        render_selection_box(selection->selection_renderer, selection->ax, selection->ay, selection->bx, selection->by, color);

        if (selection->len > 1) {
            if (selection->is_hovering_handle)
                vec3_set(color, 1.0f, 1.0f, 1.0f);
            else
                vec3_set(color, 0.5f, 0.5f, 0.5f);
            
            render_selection_handle(selection->selection_renderer, selection->bx, selection->by, HANDLE_SIZE, color);
        }
    }
    
    if (selection->len == 0)
        return;
    
    render_model_vertices_selection(selection->vertex_renderer, &model, selection->indices, selection->len);
    render_model_edges_selection(selection->edge_renderer, &model, selection->faces, selection->faces_len);

    render_control_point(selection->control_renderer, selection->midpoint, (vec3){1.0f, 0.0f, 1.0f});
    render_control_point(selection->control_renderer, selection->offset, (vec3){0.0f, 1.0f, 1.0f});
}

void update_selection_menu() {
    static char buffer[128];
    
    if (!igBegin("Edit Verticess", NULL, ImGuiWindowFlags_NoCollapse))
        return;
    
    sprintf(buffer, "Num. Vertices: %d", selection->len);
    igText(buffer);

    sprintf(buffer, "Sel. Vertices: ");
    for (int i = 0; i < selection->len; i++)
        sprintf(buffer, "%s%d ", buffer, selection->indices[i]);
    igText(buffer);

    char *coplanar_str = selection->len > 1 ? (selection->is_coplanar ? "Yes" : "No") : "--";
    sprintf(buffer, "Coplanar: %-5s", coplanar_str);
    igText(buffer);

    if (selection->is_coplanar) {
        igSameLine(0, 10);
        if (igButton("Extend", (struct ImVec2){ 100, 0 }))
            extend_selection(selection);
    }

    if (selection->faces_len > 0) {
        igSeparator();
        
        sprintf(buffer, "Num. Faces: %d", selection->faces_len);
        igText(buffer);

        sprintf(buffer, "Sel. Faces: ");
        for (int i = 0; i < selection->faces_len; i++)
            sprintf(buffer, "%s%d ", buffer, selection->faces[i]);
        igText(buffer);
        
        if (igButton("Flip Face(s)", (struct ImVec2){ 0, 0 }))
            for (int i = 0; i < selection->faces_len; i++)
                flip_face(&model, selection->faces[i]);
        
        igSameLine(0, 10);
        igText("Reverse the vertex order of the face");
        
        float *color = model.faces[selection->faces[0]].color;
        igColorEdit3("Colour", color, ImGuiColorEditFlags_Float);

        static bool apply_to_all = false;
        apply_to_all = (selection->faces_len == 1) ? false : apply_to_all;

        if (selection->faces_len > 1)
            igCheckbox("Apply to all selected faces", &apply_to_all);
        
        for (int i = 1; apply_to_all && i < selection->faces_len; i++)
            vec3_copy(model.faces[selection->faces[i]].color, color);
    }
    
    igEnd();
}

void update_selection_midpoint() {
    vec3 midpoint;
    vec3_zero(midpoint);

    for (int i = 0; i < selection->len; i++)
        vec3_add(midpoint, midpoint, model.vertices[selection->indices[i]]);

    vec3_scale(selection->midpoint, midpoint, 1.0f / (float)selection->len);
}

// Update selection corners to show movable selection area
void update_selection_box() {
    // in clip space [-1, 1]
    float tl_x = 1;
    float tl_y = 1;
    
    float br_x = -1;
    float br_y = -1;

    mat4x4 mvp;
    get_view_projection_matrix(&camera, mvp);
    
    for (int i = 0; i < selection->len; i++) {
        vec4 v;
        vec4_from_vec3(v, model.vertices[selection->indices[i]], 1);
        
        mat4x4_mul_vec4(v, mvp, v);
        vec4_scale(v, v, 1 / v[3]); // perspective divide

        if (v[0] < tl_x) tl_x = v[0];
        if (v[0] > br_x) br_x = v[0];
        if (v[1] < tl_y) tl_y = v[1];
        if (v[1] > br_y) br_y = v[1];
    }

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    int padding = 20;

    selection->ax = (tl_x + 1) / 2.0f * width - padding;
    selection->ay = (1 - tl_y) / 2.0f * height + padding;
    selection->bx = (br_x + 1) / 2.0f * width + padding;
    selection->by = (1 - br_y) / 2.0f * height - padding;
}

void clear_selection() {
    selection->len = 0;
    selection->faces_len = 0;

    selection->state = INITIAL;
}

void update_selection_faces() {
    bool sparse_map[model.vertices_len];
    memset(sparse_map, false, sizeof(sparse_map));

    for (int i = 0; i < selection->len; i++)
        sparse_map[selection->indices[i]] = true;
    
    selection->faces_len = 0;
    for (int i = 0; i < model.faces_len; i++) {
        face_t *face = &model.faces[i];
        
        bool is_included = true;
        for (int j = 0; j < face->len && is_included; j++)
            is_included = sparse_map[face->indices[j]];

        if (!is_included)
            continue;

        if (selection->faces_len == selection->faces_cap) {
            selection->faces_cap *= 2;
            selection->faces = (uint32_t*)realloc(selection->faces, sizeof(uint32_t) * selection->faces_cap);
        }

        selection->faces[selection->faces_len++] = i;
    }
}

void extend_selection() {
    uint32_t sorted_indices[selection->len];
    uint32_t extend_indices[selection->len];

    vec3 normal;
    calculate_normal(&model, normal, selection->indices, selection->len);

    for (int i = 0; i < selection->len; i++)
        sorted_indices[i] = selection->indices[i];

    sort_by_angle(&model, selection->midpoint, normal, sorted_indices, selection->len);
    
    for (int i = 0; i < selection->len; i++) {
        vec3 v;
        vec3_add(v, model.vertices[sorted_indices[i]], normal);

        extend_indices[i] = add_vertex(&model, v);
    }

    for (int i = 0; i < selection->len; i++) {
        uint32_t ia = sorted_indices[i];
        uint32_t ib = sorted_indices[(i + 1) % selection->len];
        uint32_t ic = extend_indices[(i + 1) % selection->len];
        uint32_t id = extend_indices[i];
        
        uint32_t face_index = add_face_quad(&model, ia, ib, ic, id);
        vec3_set(model.faces[face_index].color, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f);
    }

    // TODO: Update selection
}

bool point_inside_selection(double x, double y) {
    float min_x = fminf(selection->ax, selection->bx);
    float min_y = fminf(selection->ay, selection->by);

    float max_x = fmaxf(selection->ax, selection->bx);
    float max_y = fmaxf(selection->ay, selection->by);

    return (min_x < x && x < max_x) && (min_y < y && y < max_y);
}

bool point_inside_handle(double x, double y) {
    float side = HANDLE_SIZE / 2.0f; 

    float min_x = selection->bx - side;
    float min_y = selection->by - side;
    
    float max_x = selection->bx + side;
    float max_y = selection->by + side;

    return (min_x < x && x < max_x) && (min_y < y && y < max_y);
}
