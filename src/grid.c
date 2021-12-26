#include "grid.h"

grid_t grid;
extern int width, height;

void init_grid() {
    // clang-format off
    vec3 vertices[] = {
        {-0.5f, 0.0f, -0.5f},
        {+0.5f, 0.0f, -0.5f},
        {+0.5f, 0.0f, +0.5f},
        {-0.5f, 0.0f, +0.5f},
        {-0.5f, 1.0f, -0.5f},
        {+0.5f, 1.0f, -0.5f},
        {+0.5f, 1.0f, +0.5f},
        {-0.5f, 1.0f, +0.5f},
        {+0.0f, 0.0f, -0.5f},
        {+0.0f, 0.0f, +0.5f},
        {-0.5f, 0.0f, +0.0f},
        {+0.5f, 0.0f, +0.0f},
    };  

    uint32_t indices[] = {
        0, 1, 1, 2, 2, 3, 3, 0,
        0, 4, 1, 5, 2, 6, 3, 7,
        4, 5, 5, 6, 6, 7, 7, 4,
        8, 9, 10, 11
    };
    // clang-format on

    glGenVertexArrays(1, &grid.vao);
    glBindVertexArray(grid.vao);

    // Vertices
    glGenBuffers(1, &grid.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, grid.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Indices
    glGenBuffers(1, &grid.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grid.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);  // Attrib pointer for currently bound buffer
}

void draw_grid(vec3 camera_pos, int shader) {
    glUseProgram(shader);

    mat4x4 model, view, projection;
    mat4x4_identity(model);
    mat4x4_look_at(view, camera_pos, (vec3){0, 0, 0}, (vec3){0, 1, 0});
    mat4x4_perspective(projection, 45.0f, (float)width / (float)height, 0.1f, 100.0f);

    GLint model_loc = glGetUniformLocation(shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float *)model);

    GLint view_loc = glGetUniformLocation(shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float *)view);

    GLint projection_loc = glGetUniformLocation(shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float *)projection);

    GLint color_loc = glGetUniformLocation(shader, "color");
    glUniform3f(color_loc, 0.5f, 0.5f, 0.5f);

    glBindVertexArray(grid.vao);
    glDrawElements(GL_LINES, 32, GL_UNSIGNED_INT, 0);
}

void free_grid() {
    glDeleteVertexArrays(1, &grid.vao);
    glDeleteBuffers(1, &grid.vbo);
}
