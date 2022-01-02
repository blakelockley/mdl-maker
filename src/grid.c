#include "grid.h"

#include "camera.h"
#include "shader.h"

grid_t grid;
extern camera_t camera;
extern int width, height;

void init_grid() {
    vec3 vertices[404];

    for (int i = 0; i < 50; i++) {
        vec3_set(vertices[000 + (i * 2) + 0], (i - 50.0f) / 10.0f, -0.5f, -5.0f);
        vec3_set(vertices[000 + (i * 2) + 1], (i - 50.0f) / 10.0f, -0.5f, +5.0f);

        vec3_set(vertices[100 + (i * 2) + 0], (float)i / 10.0f + 0.1f, -0.5f, -5.0f);
        vec3_set(vertices[100 + (i * 2) + 1], (float)i / 10.0f + 0.1f, -0.5f, +5.0f);

        vec3_set(vertices[200 + (i * 2) + 0], -5.0f, -0.5f, (i - 50.0f) / 10.0f);
        vec3_set(vertices[200 + (i * 2) + 1], +5.0f, -0.5f, (i - 50.0f) / 10.0f);

        vec3_set(vertices[300 + (i * 2) + 0], -5.0f, -0.5f, (float)i / 10.0f + 0.1f);
        vec3_set(vertices[300 + (i * 2) + 1], +5.0f, -0.5f, (float)i / 10.0f + 0.1f);
    }

    // x-axis
    vec3_set(vertices[400], -5.0f, -0.5f, 0.0f);
    vec3_set(vertices[401], +5.0f, -0.5f, 0.0f);

    // z-axis
    vec3_set(vertices[402], 0.0f, -0.5f, -5.0f);
    vec3_set(vertices[403], 0.0f, -0.5f, +5.0f);

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
    mat4x4_look_at(view, camera.pos, camera.dir, camera.up);
    mat4x4_perspective(projection, 45.0f, (float)width / (float)height, 0.1f, 100.0f);

    GLint model_loc = glGetUniformLocation(grid.shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float *)model);

    GLint view_loc = glGetUniformLocation(grid.shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float *)view);

    GLint projection_loc = glGetUniformLocation(grid.shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float *)projection);

    GLint color_loc = glGetUniformLocation(grid.shader, "color");
    glBindVertexArray(grid.vao);

    glUniform3f(color_loc, 0.5f, 0.5f, 0.5f);
    glDrawArrays(GL_LINES, 0, 400);

    glUniform3f(color_loc, 1.0f, 0.0f, 0.0f);
    glDrawArrays(GL_LINES, 400, 2);

    glUniform3f(color_loc, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINES, 402, 2);
}

void free_grid() {
    glDeleteVertexArrays(1, &grid.vao);
    glDeleteBuffers(1, &grid.vbo);
}
