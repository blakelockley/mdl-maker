#include "grid.h"

#include "camera.h"
#include "viewport.h"
#include "shader.h"

#include <stdio.h>

grid_t grid;

extern camera_t camera;
extern int width, height;


void init_grid() {
    vec3 vertices[84];

    for (int i = 0; i < 10; i++) {
        vec3_set(vertices[00 + (i * 2) + 0], -(float)(i + 1), 0.0f, -10.0f);
        vec3_set(vertices[00 + (i * 2) + 1], -(float)(i + 1), 0.0f, +10.0f);

        vec3_set(vertices[20 + (i * 2) + 0], +(float)(i + 1), 0.0f, -10.0f);
        vec3_set(vertices[20 + (i * 2) + 1], +(float)(i + 1), 0.0f, +10.0f);

        vec3_set(vertices[40 + (i * 2) + 0], -10.0f, 0.0f, -(float)(i + 1));
        vec3_set(vertices[40 + (i * 2) + 1], +10.0f, 0.0f, -(float)(i + 1));

        vec3_set(vertices[60 + (i * 2) + 0], -10.0f, 0.0f, +(float)(i + 1));
        vec3_set(vertices[60 + (i * 2) + 1], +10.0f, 0.0f, +(float)(i + 1));
    }

    // x-axis
    vec3_set(vertices[80], -10.0f, 0.0f, 0.0f);
    vec3_set(vertices[81], +10.0f, 0.0f, 0.0f);

    // z-axis
    vec3_set(vertices[82], 0.0f, 0.0f, -10.0f);
    vec3_set(vertices[83], 0.0f, 0.0f, +10.0f);

    glGenVertexArrays(1, &grid.vao);
    glBindVertexArray(grid.vao);

    // Vertices
    glGenBuffers(1, &grid.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, grid.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);  // Attrib pointer for currently bound buffer

    grid.shader = load_shader("shaders/static.vert", "shaders/static.frag");
}

void draw_grid() {
    glUseProgram(grid.shader);

    mat4x4 model, view, projection;
    mat4x4_identity(model);
    get_view_matrix(&camera, view);
    get_projection_matrix(projection);

    GLint model_loc = glGetUniformLocation(grid.shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float *)model);

    GLint view_loc = glGetUniformLocation(grid.shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float *)view);

    GLint projection_loc = glGetUniformLocation(grid.shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float *)projection);

    GLint color_loc = glGetUniformLocation(grid.shader, "color");
    glBindVertexArray(grid.vao);

    glUniform3f(color_loc, 0.5f, 0.5f, 0.5f);
    glDrawArrays(GL_LINES, 0, 80);

    glUniform3f(color_loc, 1.0f, 0.0f, 0.0f);
    glDrawArrays(GL_LINES, 80, 2);

    glUniform3f(color_loc, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINES, 82, 2);
}

void free_grid() {
    glDeleteVertexArrays(1, &grid.vao);
    glDeleteBuffers(1, &grid.vbo);
}
