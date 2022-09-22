#include "selection.h"

#include <stdio.h>
#include <stdlib.h>

#include "camera.h"
#include "shader.h"
#include "model.h"
#include "viewport.h"

extern camera_t camera;
extern viewport_t viewport;
extern model_t model;

void update_selection(selection_t *selection);
void update_control_axis(selection_t *selection);
void get_selection_midpoint(selection_t *selection, vec3 midpoint);

void init_selection(selection_t *selection) {
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

    glGenVertexArrays(1, &selection->control_vao);
    glBindVertexArray(selection->control_vao);

    glGenBuffers(1, &selection->control_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, selection->control_vbo);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);  // Attrib pointer for currently bound buffer

    selection->control_shader = load_shader("shaders/static.vert", "shaders/static.frag");

    vec3_set(selection->control_origin, 0.0f, 1.0f, 2.0f);
    vec3_set(selection->control_axis, 1.0f, 0.0f, 0.0f);
    update_control_axis(selection);
}

void free_selection(selection_t *selection) {
    glDeleteVertexArrays(1, &selection->vao);
    glDeleteBuffers(1, &selection->vbo);
}

void render_selection(selection_t *selection) {
    glUseProgram(selection->control_shader);

    mat4x4 model, view, projection;
    mat4x4_identity(model);
    get_view_matrix(&camera, view);
    get_projection_matrix(&viewport, projection);
    
    GLint model_loc = glGetUniformLocation(selection->control_shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)model);

    GLint view_loc = glGetUniformLocation(selection->control_shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float*)view);

    GLint projection_loc = glGetUniformLocation(selection->control_shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)projection);

    glDepthFunc(GL_ALWAYS);
    glBindVertexArray(selection->control_vao);

    GLint color_loc = glGetUniformLocation(selection->control_shader, "color");
    glUniform3f(color_loc, 1.0f, 0.0f, 1.0f);

    glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, 1);

    color_loc = glGetUniformLocation(selection->control_shader, "color");
    glUniform3f(color_loc, 1.0f, 1.0f, 1.0f);
    glDrawArrays(GL_LINES, 1, 2);

    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(0);
    
    if (!selection->is_visible)
        return;

    glUseProgram(selection->shader);

    color_loc = glGetUniformLocation(selection->shader, "color");
    glUniform3f(color_loc, 0.8f, 0.4f, 0.2f);

    glBindVertexArray(selection->vao);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void handle_selection_start(selection_t *selection, double x, double y) {
    selection->is_visible = 0;
    selection->ax = x;
    selection->ay = y;

    update_selection(selection);
}

void handle_selection_move(selection_t *selection, double x, double y) {
    selection->is_visible = 1;
    selection->bx = x;
    selection->by = y;

    update_selection(selection);  
}

void handle_selection_end(selection_t *selection, double x, double y, int extend_selection) {
    selection->is_visible = 0;

    double min_x = fmin(selection->ax, selection->bx);
    double min_y = fmin(selection->ay, selection->by);

    double max_x = fmax(selection->ax, selection->bx);
    double max_y = fmax(selection->ay, selection->by);
    
    mat4x4 view, projection, mvp;
    get_view_matrix(&camera, view);
    get_projection_matrix(&viewport, projection);

    mat4x4_identity(mvp);
    mat4x4_mul(mvp, mvp, projection);
    mat4x4_mul(mvp, mvp, view);

    if (!extend_selection)
        selection->len = 0;

    for (int i = 0; i < model.vertices_len; i++) {        
        vec4 pos;
        vec4_from_vec3(pos, model.vertices[i], 1.0f);

        mat4x4_mul_vec4(pos, mvp, pos);
        vec3_scale(pos, pos, 1.0f / pos[3]); // Perspective divide

        double x = pos[0];
        double y = pos[1];

        // Check if vertex is in selction bounds
        if (!(min_x <= x && x <= max_x && min_y <= y && y <= max_y))
            continue;

        // Check if vertex index is already in selection indices
        int already_exists = 0;
        for (int j = 0; j < selection->len && !already_exists; j++)
            already_exists = (selection->indices[j] == i);

        if (!already_exists)
            selection->indices[selection->len++] = i;
    }
    
    update_selection(selection);

    get_selection_midpoint(selection, selection->control_origin);
    update_control_axis(selection);
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

void update_selection(selection_t *selection) {
    vec2 vertices[4];

    vec2_set(vertices[0], selection->ax, selection->ay);
    vec2_set(vertices[1], selection->bx, selection->ay);
    vec2_set(vertices[2], selection->bx, selection->by);
    vec2_set(vertices[3], selection->ax, selection->by);

    glBindVertexArray(selection->vao);

    glBindBuffer(GL_ARRAY_BUFFER, selection->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void update_control_axis(selection_t *selection) {
    vec3 control_vertices[3];
    vec3_copy(control_vertices[0], selection->control_origin);

    vec3 axis_a;
    vec3_scale(axis_a, selection->control_axis, 100.0f);
    vec3_add(control_vertices[1], selection->control_origin, axis_a);

    vec3 axis_b;
    vec3_scale(axis_b, selection->control_axis, -100.0f);
    vec3_add(control_vertices[2], selection->control_origin, axis_b);

    glBindBuffer(GL_ARRAY_BUFFER, selection->control_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * 3, control_vertices, GL_DYNAMIC_DRAW);
}
