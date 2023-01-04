#include "selection.h"

#include <stdio.h>
#include <stdlib.h>

#include "camera.h"
#include "shader.h"
#include "model.h"
#include "picker.h"

#include "renderers.h"

extern camera_t camera;
extern model_t model;
extern picker_t picker;

extern GLFWwindow *window;

void buffer_selection(selection_t *selection);
void caclulate_selection_midpoint(selection_t *selection);
void select_faces(selection_t *selection);

// closest power of 2 * 10
// (1 << (uint32_t)ceilf(log2f(n))) * 10;

void init_selection(selection_t *selection) {
    selection->action = ACTION_SELECT;
    
    selection->is_visible = 0;

    selection->indices = (uint32_t*)malloc(sizeof(uint32_t) * 10);
    selection->len = 0;
    selection->cap = 10;

    selection->faces = (uint32_t*)malloc(sizeof(uint32_t) * 10);
    selection->faces_len = 0;
    selection->faces_cap = 10;

    glGenVertexArrays(1, &selection->vao);
    glBindVertexArray(selection->vao);

    // Vertices
    glGenBuffers(1, &selection->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, selection->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void *)0);  // Attrib pointer for currently bound buffer

    selection->shader = load_shader("shaders/gui.vert", "shaders/gui.frag");
}

void free_selection(selection_t *selection) {
    glDeleteVertexArrays(1, &selection->vao);
    glDeleteBuffers(1, &selection->vbo);
}

void render_selection(selection_t *selection, renderer_t *vertex_renderer, renderer_t *edge_renderer) {
    render_model_vertices_selection(vertex_renderer, &model, selection->indices, selection->len);
    render_model_edges_selection(edge_renderer, &model, selection->faces, selection->faces_len);
    
    if (!selection->is_visible)
        return;

    glUseProgram(selection->shader);

    GLint color_loc = glGetUniformLocation(selection->shader, "color");
    glUniform3f(color_loc, 0.8f, 0.4f, 0.2f);

    glBindVertexArray(selection->vao);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void update_selection(selection_t *selection) {

    push_debug_point(selection->midpoint, (vec3){1.0f, 0.0f, 1.0f});
   
    if (igBegin("Edit Verticess", NULL, ImGuiWindowFlags_NoCollapse)) {
        static char buffer[128];
        
        sprintf(buffer, "Num. Vertices: %d", selection->len);
        igText(buffer);

        sprintf(buffer, "Sel. Vertices: ");
        for (int i = 0; i < selection->len; i++)
            sprintf(buffer, "%s%d ", buffer, selection->indices[i]);
        igText(buffer);
        
        char *actions[] = {"Select", "Move"};
        sprintf(buffer, "Action: %-7s", actions[selection->action]);
        igText(buffer);

        for (int i = 0; i < 2; i++) {
            igSameLine(0, 10);
            if (igButton(actions[i], (struct ImVec2){ 100, 0 })) {
                selection->action = i;
            }
        }

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
            
            sprintf(buffer, "Num. Face: %d", selection->faces_len);
            igText(buffer);

            sprintf(buffer, "Sel. Vertices: ");
            for (int i = 0; i < selection->faces_len; i++)
                sprintf(buffer, "%s%d ", buffer, selection->faces[i]);
            igText(buffer);
            
            igText("Reverse the vertex order of the face");
            if (igButton("Flip Face(s)", (struct ImVec2){ 0, 0 }))
                for (int i = 0; i < selection->faces_len; i++)
                    flip_face(&model, selection->faces[i]);
            
            float *color = model.faces[selection->faces[0]].color;
            igColorEdit3("Colour", color, ImGuiColorEditFlags_Float);

            static bool apply_to_all = false;
            apply_to_all = (selection->faces_len == 1) ? false : apply_to_all;
            igCheckbox("Apply to all face", &apply_to_all);
            
            for (int i = 1; apply_to_all && i < selection->faces_len; i++)
                vec3_copy(model.faces[selection->faces[i]].color, color);

        }
        
        igEnd();
    }
}

void handle_selection_start(selection_t *selection, float x, float y, bool shift_pressed) {
    selection->ax = x;
    selection->ay = y;

    if (shift_pressed || selection->len == 0) {
        selection->is_visible = 0;
        buffer_selection(selection);
        
        return;
    }

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    float clip_x = (x / width) * 2.0f - 1.0f;
    float clip_y = 1.0f - (y / height) * 2.0f;
    
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
}

void handle_selection_move(selection_t *selection, float x, float y, bool shift_pressed) {
    selection->bx = x;
    selection->by = y;

    if (shift_pressed || selection->len == 0) {
        selection->is_visible = 1;
        buffer_selection(selection);
        
        return;
    }
        
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

    float clip_x = (x / width) * 2.0f - 1.0f;
    float clip_y = 1.0f - (y / height) * 2.0f;
    float clip_z = projected_midpoint[2];
    
    vec4 v, r;
    vec4_set(v, clip_x, clip_y, clip_z, 1);
    
    mat4x4_invert(inverse_vp, vp);
    mat4x4_mul_vec4(r, inverse_vp, v);
    
    vec3 plane_pos;
    vec3_from_vec4(plane_pos, r);
    vec3_sub(plane_pos, plane_pos, selection->offset);

    move_selection(selection, plane_pos);
}

void handle_selection_end(selection_t *selection, float x, float y, bool shift_pressed) {
    selection->bx = x;
    selection->by = y;

    if (shift_pressed || selection->len == 0) {
        selection->is_visible = 0;
        
        float min_x = fminf(selection->ax, selection->bx);
        float min_y = fminf(selection->ay, selection->by);

        float max_x = fmaxf(selection->ax, selection->bx);
        float max_y = fmaxf(selection->ay, selection->by);

        float width = max_x - min_x;
        float height = max_y - min_y;

        if (width < 5.0f || height < 5.0f)
            return; // selection too small

        if (!shift_pressed)
            selection->len = 0;
        
        render_picker_to_vertex_ids(&picker, &model);
        select_ids_in_rect(selection, (vec2){min_x, min_y}, (vec2){max_x, max_y});

        selection->is_coplanar = (selection->len > 1) \
            && check_coplanar_vertices(&model, selection->indices, selection->len);

        caclulate_selection_midpoint(selection);

        select_faces(selection);
    }
}

void caclulate_selection_midpoint(selection_t *selection) {
    vec3 midpoint;
    vec3_zero(midpoint);

    for (int i = 0; i < selection->len; i++)
        vec3_add(midpoint, midpoint, model.vertices[selection->indices[i]]);

    vec3_scale(selection->midpoint, midpoint, 1.0f / (float)selection->len);
}

void clear_selection(selection_t *selection) {
    selection->len = 0;
}

void append_selection(selection_t *selection, uint32_t index) {
    if (selection->len == selection->cap) {
        selection->cap *= 2;
        selection->indices = (uint32_t*)realloc(selection->indices, sizeof(uint32_t) * selection->cap);
    }

    selection->indices[selection->len++] = index;
}

void select_faces(selection_t *selection) {
    bool sparse_map[model.vertices_len];
    memset(sparse_map, false, sizeof(sparse_map));

    for (int i = 0; i < selection->len; i++)
        sparse_map[i] = true;
    
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

// Converts the coordinates from screen space: [0, width/height] to clip space: [-1.0f, 1.0f]
void normalize_mouse_pos(GLFWwindow *window, float *normal_x, float *normal_y, float mouse_x, float mouse_y) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    *normal_x = (2.0f * mouse_x) / width - 1.0f;
    *normal_y = 1.0f - (2.0f * mouse_y) / height;
}

void buffer_selection(selection_t *selection) {
    vec2 vertices[4];

    float tl_x, tl_y;
    normalize_mouse_pos(window, &tl_x, &tl_y, selection->ax, selection->ay);
    
    float br_x, br_y;
    normalize_mouse_pos(window, &br_x, &br_y, selection->bx, selection->by);

    vec2_set(vertices[0], tl_x, tl_y);
    vec2_set(vertices[1], br_x, tl_y);
    vec2_set(vertices[2], br_x, br_y);
    vec2_set(vertices[3], tl_x, br_y);

    glBindVertexArray(selection->vao);

    glBindBuffer(GL_ARRAY_BUFFER, selection->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

void scale_selection(selection_t *selection, float scale) {
    for (int i = 0; i < selection->len; i++) {
        uint32_t index = selection->indices[i];
        
        vec3 delta;
        vec3_sub(delta, model.vertices[index], selection->midpoint);
        vec3_scale(delta, delta, scale);
        
        vec3_add(model.vertices[selection->indices[i]], selection->midpoint, delta);
    }

    caclulate_selection_midpoint(selection);
}

void move_selection(selection_t *selection, vec3 location) {
    for (int i = 0; i < selection->len; i++) {
        uint32_t index = selection->indices[i];

        vec3 delta;
        vec3_sub(delta, model.vertices[index], selection->midpoint);
        
        vec3_add(model.vertices[index], location, delta);
    }

    caclulate_selection_midpoint(selection);
}

void extend_selection(selection_t *selection) {
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
