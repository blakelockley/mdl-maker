#include "selection.h"

#include <stdio.h>
#include <stdlib.h>

#include "camera.h"
#include "shader.h"
#include "model.h"
#include "picker.h"

#include "renderers.h"

#define HANDLE_SIZE 16
#define X 0
#define Y 1
#define Z 2

extern bool render_vertices;
extern bool render_edges;

extern camera_t camera;
extern model_t model;
extern picker_t picker;

extern GLFWwindow *window;

extern struct ImGuiIO* io;

static selection_t _selection;
selection_t *selection = &_selection;

void show_selection_menu();

// selection

void start_selection(double mouse_x, double mouse_y);
void continue_selection(double mouse_x, double mouse_y);
void finish_selection(double mouse_x, double mouse_y);

void create_vertex(double mouse_x, double mouse_y);

// moving

void start_moving(double mouse_x, double mouse_y);
void continue_moving(double mouse_x, double mouse_y);
void finish_moving(double mouse_x, double mouse_y);

// resizing

void start_resizing(double mouse_x, double mouse_y);
void continue_resizing(double mouse_x, double mouse_y);
void finish_resizing(double mouse_x, double mouse_y);
void finish_moving(double mouse_x, double mouse_y);

// rotating

void start_rotating(double mouse_x, double mouse_y, int axis);
void continue_rotating(double mouse_x, double mouse_y);
void finish_rotating(double mouse_x, double mouse_y);

// extending

void start_extending(double mouse_x, double mouse_y);
void continue_extending(double mouse_x, double mouse_y);
void finish_extending(double mouse_x, double mouse_y);

// calculate methods

void calculate_selection_box();
void calculate_selection_faces();
void calculate_selection_midpoint();

// utilities

void get_rotation_handle(vec3 handle, int axis);

void project_point(float *x, float *y, vec3 point);
bool point_inside_selection(double x, double y);
bool point_inside_handle(double handle_x, double handle_y, double mouse_x, double mouse_y);
bool point_inside_rotation_handle(int axis, double mouse_x, double mouse_y);

// operations

void remove_vertices();
void merge_vertices();

void init_selection(renderer_t *selection_renderer, renderer_t *control_renderer, renderer_t *vertex_renderer, renderer_t *edge_renderer) {
    selection->indices = (uint32_t*)malloc(sizeof(uint32_t) * 10);
    selection->len = 0;
    selection->cap = 10;

    selection->faces = (uint32_t*)malloc(sizeof(uint32_t) * 10);
    selection->faces_len = 0;
    selection->faces_cap = 10;

    selection->deltas = (vec3*)malloc(sizeof(vec3) * 10);
    selection->deltas_cap = 10;

    selection->selection_renderer = selection_renderer;
    selection->control_renderer = control_renderer;
    selection->vertex_renderer = vertex_renderer;
    selection->edge_renderer = edge_renderer;

    selection->show_rotate = false;
    selection->rotation_axis = -1;
    selection->rotations[X] = 0.0f;
    selection->rotations[Y] = 0.0f;
    selection->rotations[Z] = 0.0f;

    selection->allow_x = true;
    selection->allow_y = true;
    selection->allow_z = true;
    
    selection->state = INITIAL;
    selection->hovering_control = NONE;
}

void free_selection() {
    free(selection->indices);
    free(selection->faces);
}

void update_selection() {
    if (selection->len > 0)
        show_selection_menu();

    if (io->WantCaptureMouse)
        return;

    int action = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    
    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);

    bool in_selection = point_inside_selection(mouse_x, mouse_y);
    
    if (selection->len == 1) {
        if (in_selection)
            selection->hovering_control = SELECTION;
        else
            selection->hovering_control = NONE;

    } else if (selection->len > 1) {
        if (point_inside_handle(selection->bx, selection->by, mouse_x, mouse_y))
            selection->hovering_control = RESIZE;

        else if (selection->show_rotate && selection->allow_x && point_inside_rotation_handle(X, mouse_x, mouse_y))
            selection->hovering_control = ROTATE_X;

        else if (selection->show_rotate && selection->allow_y && point_inside_rotation_handle(Y, mouse_x, mouse_y))
            selection->hovering_control = ROTATE_Y;

        else if (selection->show_rotate && selection->allow_z && point_inside_rotation_handle(Z, mouse_x, mouse_y))
            selection->hovering_control = ROTATE_Z;

        else if (in_selection)
            selection->hovering_control = SELECTION;

        else
            selection->hovering_control = NONE;
    }

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
        calculate_selection_box();
        calculate_selection_midpoint();

        if (action == GLFW_PRESS) {
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                start_selection(mouse_x, mouse_y);
                break;
            }

            switch (selection->hovering_control) {
            case SELECTION:
                start_moving(mouse_x, mouse_y);
                break;

            case RESIZE:
                start_resizing(mouse_x, mouse_y);
                break;

            case ROTATE_X:
                start_rotating(mouse_x, mouse_y, X);
                break;

            case ROTATE_Y:
                start_rotating(mouse_x, mouse_y, Y);
                break;

            case ROTATE_Z:
                start_rotating(mouse_x, mouse_y, Z);
                break;
            
            default:
                start_selection(mouse_x, mouse_y);
                break;
            }
        }
        
        break;

    case MOVING:
        calculate_selection_box();
        calculate_selection_midpoint();
        
        if (action == GLFW_PRESS)
            continue_moving(mouse_x, mouse_y);

        if (action == GLFW_RELEASE)
            finish_moving(mouse_x, mouse_y);
        
        break;

    case RESIZING:
        calculate_selection_box();
        calculate_selection_midpoint();
        
        if (action == GLFW_PRESS)
            continue_resizing(mouse_x, mouse_y);

        if (action == GLFW_RELEASE)
            finish_resizing(mouse_x, mouse_y);

        break;

    case ROTATING:
        calculate_selection_box();
        calculate_selection_midpoint();
        
        if (action == GLFW_PRESS)
            continue_rotating(mouse_x, mouse_y);

        if (action == GLFW_RELEASE)
            finish_rotating(mouse_x, mouse_y);
        
        break;
    
    default:
        break;
    }
}

void render_selection() {
    vec3 active_colour = { 1.0f, 1.0f, 1.0f };
    
    if (selection->state == SELECTING)
        render_selection_box(selection->selection_renderer, selection->ax, selection->ay, selection->bx, selection->by, (vec3){0.8f, 0.4f, 0.2f});

    if (selection->state == SELECTED) {
        vec3 selection_colour = { 0.5f, 0.5f, 0.5f };
        vec3 handle_colour    = { 0.5f, 0.5f, 0.5f };
        vec3 rotate_x_colour  = { 1.0f, 0.0f, 0.0f };
        vec3 rotate_y_colour  = { 0.0f, 1.0f, 0.0f };
        vec3 rotate_z_colour  = { 0.0f, 0.0f, 1.0f };
        
        switch (selection->hovering_control) {
        case SELECTION:
            vec3_copy(selection_colour, active_colour);
            break;

        case RESIZE:
            vec3_copy(handle_colour, active_colour);
            break;

        case ROTATE_X:
            vec3_copy(rotate_x_colour, active_colour);
            break;

        case ROTATE_Y:
            vec3_copy(rotate_y_colour, active_colour);
            break;

        case ROTATE_Z:
            vec3_copy(rotate_z_colour, active_colour);
            break;
        
        default:
            break;
        }

        render_selection_box(selection->selection_renderer, selection->ax, selection->ay, selection->bx, selection->by, selection_colour);
        
        if (selection->len > 1) {
            render_selection_handle(selection->selection_renderer, selection->bx, selection->by, HANDLE_SIZE, handle_colour);

            if (selection->show_rotate) {
                if (selection->allow_x) {
                    vec3 x_handle;
                    get_rotation_handle(x_handle, X);

                    render_control_circle(selection->control_renderer, selection->midpoint, (vec3){1.0f, 0.0f, 0.0f}, 0.1f, rotate_x_colour);
                    render_control_point(selection->control_renderer, x_handle, HANDLE_SIZE, rotate_x_colour);
                }

                if (selection->allow_y) {
                    vec3 y_handle;
                    get_rotation_handle(y_handle, Y);

                    render_control_circle(selection->control_renderer, selection->midpoint, (vec3){0.0f, 1.0f, 0.0f}, 0.1f, rotate_y_colour);
                    render_control_point(selection->control_renderer, y_handle, HANDLE_SIZE, rotate_y_colour);
                }

                if (selection->allow_z) {
                    vec3 z_handle;
                    get_rotation_handle(z_handle, Z);

                    render_control_circle(selection->control_renderer, selection->midpoint, (vec3){0.0f, 0.0f, 1.0f}, 0.1f, rotate_z_colour);
                    render_control_point(selection->control_renderer, z_handle, HANDLE_SIZE, rotate_z_colour);
                }
            }
        }
    }

    if (selection->state == MOVING) {
        render_selection_box(selection->selection_renderer, selection->ax, selection->ay, selection->bx, selection->by, active_colour);
    }

    if (selection->state == RESIZING) {
        render_selection_box(selection->selection_renderer, selection->ax, selection->ay, selection->bx, selection->by, active_colour);
        render_selection_handle(selection->selection_renderer, selection->bx, selection->by, HANDLE_SIZE, active_colour);
    }

    if (selection->state == ROTATING) {
        vec3 handle;
        get_rotation_handle(handle, selection->rotation_axis);

        vec3 axis = { 0.0f, 0.0f, 0.0f };
        axis[selection->rotation_axis] = 1.0f;
        
        render_control_circle(selection->control_renderer, selection->midpoint, axis, 0.1f, active_colour);
        render_control_point(selection->control_renderer, handle, HANDLE_SIZE, active_colour);
    }
    
    if (selection->len == 0)
        return;
    
    if (render_vertices)
        render_model_vertices_selection(selection->vertex_renderer, &model, selection->indices, selection->len);
    
    if (render_edges)
        render_model_edges_selection(selection->edge_renderer, &model, selection->faces, selection->faces_len);
    
    render_control_point(selection->control_renderer, selection->midpoint, 20.0f, (vec3){1.0f, 0.0f, 1.0f});
}

// selection

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

    selection->is_coplanar = check_coplanar_vertices(&model, selection->indices, selection->len);
    calculate_selection_faces();
}

void start_moving(double mouse_x, double mouse_y) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    mat4x4 mvp, inverse_mvp;
    get_view_projection_matrix(&camera, mvp);
    mat4x4_invert(inverse_mvp, mvp);

    vec3 projected_midpoint;
    mat4x4_mul_vec3(projected_midpoint, mvp, selection->midpoint);

    float clip_x = (mouse_x / width) * 2.0f - 1.0f;
    float clip_y = 1.0f - (mouse_y / height) * 2.0f;
    float clip_z = projected_midpoint[2];
    
    vec3 clip;
    vec3_set(clip, clip_x, clip_y, clip_z);

    vec3 plane_pos;
    mat4x4_mul_vec3(plane_pos, inverse_mvp, clip);

    vec3_sub(selection->offset, plane_pos, selection->midpoint);

    // Capture deltas
    
    if (selection->len > selection->deltas_cap) {
        while (selection->len > selection->deltas_cap)
            selection->deltas_cap *= 2;
    
        selection->deltas = (vec3*) realloc(selection->deltas, sizeof(vec3) * selection->deltas_cap);
    }

    for (int i = 0; i < selection->len; i++)
        vec3_sub(selection->deltas[i], model.vertices[selection->indices[i]], selection->midpoint);

    selection->state = MOVING;
}

void continue_moving(double mouse_x, double mouse_y) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    mat4x4 mvp, inverse_mvp;
    get_view_projection_matrix(&camera, mvp);
    mat4x4_invert(inverse_mvp, mvp);

    vec3 projected_midpoint;
    mat4x4_mul_vec3(projected_midpoint, mvp, selection->midpoint);

    float clip_x = (mouse_x / width) * 2.0f - 1.0f;
    float clip_y = 1.0f - (mouse_y / height) * 2.0f;
    float clip_z = projected_midpoint[2];
    
    vec3 clip;
    vec3_set(clip, clip_x, clip_y, clip_z);

    vec3 plane_pos;
    mat4x4_mul_vec3(plane_pos, inverse_mvp, clip);

    vec3_sub(plane_pos, plane_pos, selection->offset);

    if (!selection->allow_x)
        plane_pos[0] = selection->midpoint[0];

    if (!selection->allow_y)
        plane_pos[1] = selection->midpoint[1];

    if (!selection->allow_z)
        plane_pos[2] = selection->midpoint[2];

    for (int i = 0; i < selection->len; i++)
        vec3_add(model.vertices[selection->indices[i]], plane_pos, selection->deltas[i]);

    calculate_selection_midpoint();
    recalculate_faces(&model);
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

    recalculate_faces(&model);
}

void finish_resizing(double mouse_x, double mouse_y) {
    selection->state = SELECTED;
}

void start_rotating(double mouse_x, double mouse_y, int axis) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    float clip_x = (mouse_x / width) * 2.0f - 1.0f;
    float clip_y = 1.0f - (mouse_y / height) * 2.0f;
    
    mat4x4 mvp;
    get_view_projection_matrix(&camera, mvp);

    vec3 projected_midpoint;
    mat4x4_mul_vec3(projected_midpoint, mvp, selection->midpoint);

    float mid_x = projected_midpoint[0];
    float mid_y = projected_midpoint[1];
    
    float x = clip_x - mid_x;
    float y = clip_y - mid_y;

    float dir = camera.dir[axis];
    dir = dir != 0.0f ? (dir / fabsf(dir)) : 1.0f;
    
    selection->state = ROTATING;
    selection->rotation_axis = axis;
    
    selection->initial_angle = atan2f(y, x) * dir;
    selection->initial_rotation = selection->rotations[axis];

    // Capture deltas
    
    if (selection->len > selection->deltas_cap) {
        while (selection->len > selection->deltas_cap)
            selection->deltas_cap *= 2;
    
        selection->deltas = (vec3*) realloc(selection->deltas, sizeof(vec3) * selection->deltas_cap);
    }

    for (int i = 0; i < selection->len; i++)
        vec3_sub(selection->deltas[i], model.vertices[selection->indices[i]], selection->midpoint);
}

void continue_rotating(double mouse_x, double mouse_y) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    float clip_x = (mouse_x / width) * 2.0f - 1.0f;
    float clip_y = 1.0f - (mouse_y / height) * 2.0f;
    
    mat4x4 mvp;
    get_view_projection_matrix(&camera, mvp);

    vec3 projected_midpoint;
    mat4x4_mul_vec3(projected_midpoint, mvp, selection->midpoint);

    float mid_x = projected_midpoint[0];
    float mid_y = projected_midpoint[1];

    float x = clip_x - mid_x;
    float y = clip_y - mid_y;

    float dir = camera.dir[selection->rotation_axis];
    dir = dir != 0.0f ? (dir / fabsf(dir)) : 1.0f;

    float angle = (atan2f(y, x) * dir) - selection->initial_angle;
    selection->rotations[selection->rotation_axis] = selection->initial_rotation + angle;

    mat4x4 rot;
    if (selection->rotation_axis == X)
        mat4x4_rotation_x(rot, -angle);

    if (selection->rotation_axis == Y)
        mat4x4_rotation_y(rot, -angle);

    if (selection->rotation_axis == Z)
        mat4x4_rotation_z(rot, -angle);

    for (int i = 0; i < selection->len; i++) {
        vec3 rotated_delta;
        mat4x4_mul_vec3(rotated_delta, rot, selection->deltas[i]);

        vec3_add(model.vertices[selection->indices[i]], selection->midpoint, rotated_delta);
    }

    recalculate_faces(&model);
}

void finish_rotating(double mouse_x, double mouse_y) {
    selection->state = SELECTED;
    selection->rotation_axis = -1;
}

void show_selection_menu() {
    static char buffer[128];

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

    const ImGuiViewport* viewport = igGetMainViewport();
    ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
    ImVec2 work_size = viewport->WorkSize;
    ImVec2 window_pos, window_pos_pivot;
    window_pos.x = work_pos.x + work_size.x - 10.0f;
    window_pos.y = work_pos.y + 10.0f;
    window_pos_pivot.x = 1.0f;
    window_pos_pivot.y = 0.0f;
    igSetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    igSetNextWindowBgAlpha(0.35f); // Transparent background
    
    if (!igBegin("Edit Verticess", NULL, window_flags))
        return;
    
    sprintf(buffer, "Num. Vertices: %d", selection->len);
    igText(buffer);

    // sprintf(buffer, "Sel. Vertices: ");
    // for (int i = 0; i < selection->len; i++)
    //     sprintf(buffer, "%s%d ", buffer, selection->indices[i]);
    // igText(buffer);

    if (igButton("Remove Vertices", (struct ImVec2){ 0, 0 })) {
        remove_vertices();
    }

    if (igButton("Merge Vertices", (struct ImVec2){ 0, 0 })) {
        merge_vertices();
    }

    char *coplanar_str = selection->len > 1 ? (selection->is_coplanar ? "Yes" : "No") : "--";
    sprintf(buffer, "Coplanar: %-5s", coplanar_str);
    igText(buffer);

    if (selection->is_coplanar) {
        igSeparator();

        if (igButton("Add Face", (struct ImVec2){ 0, 0 })) {
            add_face(&model, selection->indices, selection->len);
        }
    }

    if (selection->faces_len > 0) {
        igSeparator();
        
        sprintf(buffer, "Num. Faces: %d", selection->faces_len);
        igText(buffer);

        // sprintf(buffer, "Sel. Faces: ");
        // for (int i = 0; i < selection->faces_len; i++)
        //     sprintf(buffer, "%s%d ", buffer, selection->faces[i]);
        // igText(buffer);
        
        if (igButton("Flip Face(s)", (struct ImVec2){ 0, 0 }))
            for (int i = 0; i < selection->faces_len; i++)
                flip_face(&model, selection->faces[i]);
        
        igSameLine(0, 10);
        igText("Reverse vertex order");
        
        float *color = model.faces[selection->faces[0]].color;
        igColorEdit3("Colour", color, ImGuiColorEditFlags_Float);

        static bool apply_to_all = false;
        apply_to_all = (selection->faces_len == 1) ? false : apply_to_all;

        if (selection->faces_len > 1)
            igCheckbox("Apply to all selected faces", &apply_to_all);
        
        for (int i = 1; apply_to_all && i < selection->faces_len; i++)
            vec3_copy(model.faces[selection->faces[i]].color, color);

    }
    
    igSeparator();

    igCheckbox("Show rotation (r)", &selection->show_rotate);

    igText("Transformation axises");
    igCheckbox("X", &selection->allow_x);

    igSameLine(0, 10);
    igCheckbox("Y", &selection->allow_y);

    igSameLine(0, 10);
    igCheckbox("Z", &selection->allow_z);
    
    igEnd();
}

void clear_selection() {
    selection->len = 0;
    selection->faces_len = 0;

    selection->state = INITIAL;
}

void select_all() {
    for (int i = 0; i < model.vertices_len; i++) {
        if (selection->len == selection->cap) {
            selection->cap *= 2;
            selection->indices = (uint32_t*)realloc(selection->indices, sizeof(uint32_t) * selection->cap);
        }

        selection->indices[selection->len++] = i;
    }

    selection->state = SELECTED;
}

void calculate_selection_midpoint() {
    vec3 midpoint;
    vec3_zero(midpoint);

    for (int i = 0; i < selection->len; i++)
        vec3_add(midpoint, midpoint, model.vertices[selection->indices[i]]);

    vec3_scale(selection->midpoint, midpoint, 1.0f / (float)selection->len);
}

// Update selection corners to show movable selection area
void calculate_selection_box() {
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

void calculate_selection_faces() {
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

void start_extending(double mouse_x, double mouse_y) {
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

void continue_extending(double mouse_x, double mouse_y) {
    
}

void finish_extending(double mouse_x, double mouse_y) {

}

// utilities

void project_point(float *x, float *y, vec3 point) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    mat4x4 mvp;
    get_view_projection_matrix(&camera, mvp);

    mat4x4_mul_vec3(point, mvp, point);
    *x = (point[0] + 1) * width / 2.0f;
    *y = (1.0f - point[1]) * height / 2.0f;
}

void get_rotation_handle(vec3 handle, int axis) {
    float angle = selection->rotations[axis];
    
    switch (axis) {
    case X:
        vec3_add(handle, selection->midpoint, (vec3){0.0f, sinf(angle) * 0.1f, cosf(angle) * 0.1f});
        break;
    
    case Y:
        vec3_add(handle, selection->midpoint, (vec3){cosf(angle) * 0.1f, 0.0f, sinf(angle) * 0.1f});
        break;
    
    case Z:
        vec3_add(handle, selection->midpoint, (vec3){sinf(angle) * 0.1f, cosf(angle) * 0.1f, 0.0f});
        break;
    
    default:
        break;
    }
}

bool point_inside_selection(double x, double y) {
    float min_x = fminf(selection->ax, selection->bx);
    float min_y = fminf(selection->ay, selection->by);

    float max_x = fmaxf(selection->ax, selection->bx);
    float max_y = fmaxf(selection->ay, selection->by);

    return (min_x < x && x < max_x) && (min_y < y && y < max_y);
}

bool point_inside_handle(double handle_x, double handle_y, double mouse_x, double mouse_y) {
    float side = HANDLE_SIZE / 2.0f; 

    float min_x = handle_x - side;
    float min_y = handle_y - side;
    
    float max_x = handle_x + side;
    float max_y = handle_y + side;

    return (min_x < mouse_x && mouse_x < max_x) && (min_y < mouse_y && mouse_y < max_y);
}

bool point_inside_rotation_handle(int axis, double mouse_x, double mouse_y) {
    vec3 handle;
    get_rotation_handle(handle, axis);
    
    float handle_x, handle_y;
    project_point(&handle_x, &handle_y, handle);
    
    return point_inside_handle(handle_x, handle_y, mouse_x, mouse_y);
}

// operations

void remove_vertices() {
    bool removed_faces_map[model.faces_len];
    memset(removed_faces_map, false, sizeof(removed_faces_map));
    
    for (int i = 0; i < selection->len; i++) {
        uint32_t removed_index = selection->indices[i];

        model.vertices_len--;
        for (int j = removed_index; j < model.vertices_len; j++)
            vec3_copy(model.vertices[j],  model.vertices[j + 1]);
        
        for (int j = 0; j < model.faces_len; j++) {
            face_t *face = &model.faces[j];
            for (int k = 0; k < face->len; k++) {
                 if (face->indices[k] == removed_index)
                    removed_faces_map[j] = true;
                 
                 if (face->indices[k] > removed_index)
                    face->indices[k]--;
            }
        }
    }

    for (int i = model.faces_len - 1; i >= 0; i--) {
        if (removed_faces_map[i]) {
            model.faces_len--;
            for (int j = i; j < model.faces_len; j++)
                memcpy(&model.faces[j], &model.faces[j+1], sizeof(face_t));
        }
    }

    clear_selection();
}

void merge_vertices() {
    uint32_t target_index = selection->indices[0];

    for (int i = 1; i < selection->len; i++) {
        uint32_t merged_index = selection->indices[i];

        for (int j = 0; j < model.faces_len; j++) {
            face_t *face = &model.faces[j];
            
            for (int k = 0; k < face->len; k++) {
                 if (face->indices[k] == merged_index)
                    face->indices[k] = target_index;
            }
        }

        model.vertices_len--;
        for (int j = merged_index; j < model.vertices_len; j++)
            vec3_copy(model.vertices[j],  model.vertices[j + 1]);
        
        for (int j = 0; j < model.faces_len; j++) {
            face_t *face = &model.faces[j];
            for (int k = 0; k < face->len; k++)
                 if (face->indices[k] > merged_index)
                    face->indices[k]--;
        }
    }

    clear_selection();
}
