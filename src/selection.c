#include "selection.h"

#include <stdio.h>
#include <stdlib.h>

#include "camera.h"
#include "shader.h"
#include "model.h"
#include "picker.h"
#include "transform.h"

#include "renderers.h"

extern camera_t camera;
extern model_t model;
extern picker_t picker;
extern transform_t transform;

extern GLFWwindow *window;

void buffer_selection(selection_t *selection);
void update_control_axis(selection_t *selection);
void get_selection_midpoint(selection_t *selection, vec3 midpoint);

// closest power of 2 * 10
// (1 << (uint32_t)ceilf(log2f(n))) * 10;

void init_selection(selection_t *selection) {
    selection->mode = MODE_VERTEX;
    selection->action = ACTION_SELECT;
    
    selection->is_visible = 0;

    selection->indices = (uint32_t*)malloc(sizeof(uint32_t) * 10);
    selection->len = 0;
    selection->cap = 10;

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

void render_selection(selection_t *selection) {
    if (!selection->is_visible)
        return;

    glUseProgram(selection->shader);

    GLint color_loc = glGetUniformLocation(selection->shader, "color");
    glUniform3f(color_loc, 0.8f, 0.4f, 0.2f);

    glBindVertexArray(selection->vao);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void update_selection(selection_t *selection) {
    if (selection->len > 0 && selection->mode == MODE_VERTEX) {    
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
                    start_extend(&transform);
            }

            // igText("Transformation");

            // igSliderFloat("Translation", &transform.translation_delta, -5.0f, 5.0f, "%.2f", 0);
            // igSliderFloat("Rotation", &transform.rotation_delta, -5.0f, 5.0f, "%.2f", 0);
            // igSliderFloat("Scale", &transform.scale, 0.0f, 5.0f, "%.2f", 0);

            // if (igButton("Reset", (struct ImVec2){ 0, 0 })) {
            //     reset_transform(&transform);
            // }    

            igEnd();
        }
        
        // TODO: I don't think this is the best way to handle transformations
        // apply_transform(&transform);

        push_debug_point(transform.midpoint, (vec3){1.0f, 0.0f, 0.0f});
        
        vec3 grab_point;
        vec3_add(grab_point, transform.midpoint, transform.offset);

        push_debug_point(grab_point, (vec3){1.0f, 1.0f, 0.0f});
    }
    
    if (selection->len > 0 && selection->mode == MODE_FACE) {    
        float *color = model.faces[selection->indices[0]].color;

        if (igBegin("Edit Face(s)", NULL, ImGuiWindowFlags_NoCollapse)) {
            static char buffer[32];
            
            sprintf(buffer, "Num. Face: %d", selection->len);
            igText(buffer);

            igSeparator();
            
            igText("Reverse the vertex order of the face");
            if (igButton("Flip Face(s)", (struct ImVec2){ 0, 0 }))
                for (int i = 0; i < selection->len; i++)
                    flip_face(&model, selection->indices[i]);
            
            igSeparator();
            
            igColorPicker3("Colour", color, 0);

            static bool apply_to_all = false;
            apply_to_all = (selection->len == 1) ? false : apply_to_all;
            igCheckbox("Apply to all face", &apply_to_all);
            
            for (int i = 1; apply_to_all && i < selection->len; i++)
                vec3_copy(model.faces[selection->indices[i]].color, color);
            
            igEnd();
        }
    }
}

void handle_selection_start(selection_t *selection, float x, float y) {
    selection->ax = x;
    selection->ay = y;
    
    if (selection->action == ACTION_SELECT) {
        selection->is_visible = 0;

        buffer_selection(selection);
    }
    
    else if (selection->action == ACTION_MOVE) {
        mat4x4 view, projection, vp, inverse_vp;
        get_view_matrix(&camera, view);
        get_projection_matrix(&camera, projection);
        
        mat4x4_mul(vp, projection, view);

        vec4 midpoint, projected_midpoint;
        vec4_from_vec3(midpoint, transform.midpoint, 1);
        
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

        vec3_sub(transform.offset, plane_pos, transform.midpoint);

        start_transform(&transform, &model, selection);
    }
}

void handle_selection_move(selection_t *selection, float x, float y) {
    selection->bx = x;
    selection->by = y;
    
    if (selection->action == ACTION_SELECT) {
        selection->is_visible = 1;

        buffer_selection(selection);
    }
    
    else if (selection->action == ACTION_MOVE) {
        mat4x4 view, projection, vp, inverse_vp;
        get_view_matrix(&camera, view);
        get_projection_matrix(&camera, projection);
        
        mat4x4_mul(vp, projection, view);

        vec4 midpoint, projected_midpoint;
        vec4_from_vec3(midpoint, transform.midpoint, 1);
        
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
        vec3_sub(plane_pos, plane_pos, transform.offset);

        vec3_sub(transform.translation_axis, plane_pos, transform.midpoint);
        transform.translation_delta = 1.0f;

        apply_transform(&transform);
    }
}

void handle_selection_end(selection_t *selection, float x, float y) {
    if (selection->action == ACTION_SELECT) {
        selection->is_visible = 0;
        selection->bx = x;
        selection->by = y;
        
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

        selection->is_coplanar = (selection->len > 1) \
            && check_coplanar_vertices(&model, selection->indices, selection->len);

        start_transform(&transform, &model, selection);

        selection->action = ACTION_MOVE;
    }
    
    else if (selection->action == ACTION_MOVE) {
        // selection->action = ACTION_SELECT;
    }
}

void get_selection_midpoint(selection_t *selection, vec3 midpoint) {
    vec3_zero(midpoint);

    for (int i = 0; i < selection->len; i++)
        vec3_add(midpoint, midpoint, model.vertices[selection->indices[i]]);

    vec3_scale(midpoint, midpoint, 1.0f / (float)selection->len);
}

void clear_selection(selection_t *selection) {
    selection->len = 0;
}

void extend_selection(selection_t *selection, uint32_t index) {
    if (selection->len == selection->cap) {
        selection->cap *= 2;
        selection->indices = (uint32_t*)realloc(selection->indices, sizeof(uint32_t) * selection->cap);
    }

    selection->indices[selection->len++] = index;
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
