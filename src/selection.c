#include "selection.h"

#include <stdio.h>

#include "camera.h"
#include "shader.h"

selection_t selection;
extern camera_t camera;

int selection_len = 0;
int selection_buffer[256];

void init_selection() {
    selection.visible = 0;
    selection.existing_count = 0;

    glGenVertexArrays(1, &selection.vao);
    glBindVertexArray(selection.vao);

    // Vertices
    glGenBuffers(1, &selection.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, selection.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void *)0);  // Attrib pointer for currently bound buffer

    selection.shader = load_shader("shaders/gui.vert", "shaders/gui.frag");
}

void draw_selection() {
    if (!selection.visible)
        return;

    glUseProgram(selection.shader);

    GLint color_loc = glGetUniformLocation(selection.shader, "color");
    glUniform3f(color_loc, 1.0f, 0.0f, 1.0f);

    glBindVertexArray(selection.vao);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void free_selection() {
    glDeleteVertexArrays(1, &selection.vao);
    glDeleteBuffers(1, &selection.vbo);
}

void buffer_selection() {
    vec2 vertices[4];

    vec2_set(vertices[0], selection.ax, selection.ay);
    vec2_set(vertices[1], selection.bx, selection.ay);
    vec2_set(vertices[2], selection.bx, selection.by);
    vec2_set(vertices[3], selection.ax, selection.by);

    glBindVertexArray(selection.vao);

    glBindBuffer(GL_ARRAY_BUFFER, selection.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
}

void set_selection_start(double x, double y, int include_existing) {
    selection.ax = x;
    selection.ay = y;

    selection.visible = 0;
    selection.existing_count = include_existing ? selection_len : 0;

    buffer_selection();
}

void set_selection_end(double x, double y, object_t *object) {
    selection.bx = x;
    selection.by = y;

    double min_x = fmin(selection.ax, selection.bx);
    double min_y = fmin(selection.ay, selection.by);

    double max_x = fmax(selection.ax, selection.bx);
    double max_y = fmax(selection.ay, selection.by);

    mat4x4 model, view, projection, mvp;
    mat4x4_identity(model);
    mat4x4_look_at(view, camera.pos, camera.dir, camera.up);
    get_projection_matrix(projection);

    mat4x4_identity(mvp);
    mat4x4_mul(mvp, mvp, projection);
    mat4x4_mul(mvp, mvp, view);
    mat4x4_mul(mvp, mvp, model);

    selection_len = selection.existing_count;
    for (int i = 0; i < object->positions_len; i++) {
        vec4 pos;
        vec4_from_vec3(pos, object->positions[i], 1.0f);

        mat4x4_mul_vec4(pos, mvp, pos);
        vec3_scale(pos, pos, 1.0f / pos[3]);

        double x = pos[0];
        double y = pos[1];

        if (min_x <= x && x <= max_x && min_y <= y && y <= max_y)
            selection_buffer[selection_len++] = i;
    }

    selection.visible = 1;
    buffer_selection();
}
