#include "select.h"

#include <stdio.h>
#include <stdlib.h>

#include "camera.h"
#include "shader.h"
#include "model.h"
#include "viewport.h"

select_t select;
extern camera_t camera;
extern model_t model;

void buffer_select();

void init_select() {
    select.is_visible = 0;

    select.selection_buffer = (int*)malloc(sizeof(int) * 10);
    select.selection_len = 0;
    select.selection_cap = 10;

    glGenVertexArrays(1, &select.vao);
    glBindVertexArray(select.vao);

    // Vertices
    glGenBuffers(1, &select.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, select.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void *)0);  // Attrib pointer for currently bound buffer

    select.shader = load_shader("shaders/gui.vert", "shaders/gui.frag");
}

void draw_select() {
    if (!select.is_visible)
        return;

    glUseProgram(select.shader);

    GLint color_loc = glGetUniformLocation(select.shader, "color");
    glUniform3f(color_loc, 0.8f, 0.4f, 0.2f);

    glBindVertexArray(select.vao);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void free_select() {
    glDeleteVertexArrays(1, &select.vao);
    glDeleteBuffers(1, &select.vbo);
}

void set_select_start(double x, double y) {
    select.is_visible = 0;
    select.ax = x;
    select.ay = y;

    buffer_select();
}

void set_select_move(double x, double y) {
    select.is_visible = 1;
    select.bx = x;
    select.by = y;

    buffer_select();  
}

void set_select_end(double x, double y) {
    select.is_visible = 0;

    double min_x = fmin(select.ax, select.bx);
    double min_y = fmin(select.ay, select.by);

    double max_x = fmax(select.ax, select.bx);
    double max_y = fmax(select.ay, select.by);
    
    mat4x4 view, projection, mvp;
    get_view_matrix(view);
    get_projection_matrix(projection);

    mat4x4_identity(mvp);
    mat4x4_mul(mvp, mvp, projection);
    mat4x4_mul(mvp, mvp, view);

    select.selection_len = 0;
    for (int i = 0; i < model.vertices_len; i++) {
        vec4 pos;
        vec4_from_vec3(pos, model.vertices[i], 1.0f);

        mat4x4_mul_vec4(pos, mvp, pos);
        vec3_scale(pos, pos, 1.0f / pos[3]); // Perspective divide

        double x = pos[0];
        double y = pos[1];

        if (min_x <= x && x <= max_x && min_y <= y && y <= max_y)
            select.selection_buffer[select.selection_len++] = i;
    }
    
    buffer_select();  
}

void get_selection_midpoint(vec3 midpoint) {
    vec3_zero(midpoint);

    for (int i = 0; i < select.selection_len; i++)
        vec3_add(midpoint, midpoint, model.vertices[select.selection_buffer[i]]);

    vec3_scale(midpoint, midpoint, 1.0f / (float)select.selection_len);
}

void move_selection(vec3 delta) {
    for (int i = 0; i < select.selection_len; i++) {
        int index = select.selection_buffer[i];
        vec3_add(model.vertices[index], model.vertices[index], delta);
    }
}

void move_selection_to_position(vec3 position) {
    vec3 midpoint;
    get_selection_midpoint(midpoint);

    vec3 deltas[select.selection_len];
    for (int i = 0; i < select.selection_len; i++)
        vec3_sub(deltas[i], model.vertices[select.selection_buffer[i]], midpoint);

    vec3 new_vertices[select.selection_len];
    for (int i = 0; i < select.selection_len; i++)
        vec3_add(new_vertices[i], position, deltas[i]);

    for (int i = 0; i < select.selection_len; i++)
        vec3_copy(model.vertices[select.selection_buffer[i]], new_vertices[i]);
}

void clear_selection() {
    select.selection_len = 0;
}

void add_index_to_selection(uint32_t index) {
    select.selection_buffer[select.selection_len++] = index;
}

void buffer_select() {
    vec2 vertices[4];

    vec2_set(vertices[0], select.ax, select.ay);
    vec2_set(vertices[1], select.bx, select.ay);
    vec2_set(vertices[2], select.bx, select.by);
    vec2_set(vertices[3], select.ax, select.by);

    glBindVertexArray(select.vao);

    glBindBuffer(GL_ARRAY_BUFFER, select.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
}
