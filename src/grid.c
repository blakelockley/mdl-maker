#include "grid.h"

void init_grid(grid_t *grid) {
    vec3 vertices[(GRID_SIZE + 1) * 4];
    for (int i = 0; i <= GRID_SIZE; i++) {
        vec3_set(vertices[(i * 2) + 0], i - GRID_HALF, 0.0f, -GRID_HALF);
        vec3_set(vertices[(i * 2) + 1], i - GRID_HALF, 0.0f, +GRID_HALF);

        vec3_set(vertices[(GRID_SIZE + 1) * 2 + (i * 2) + 0], -GRID_HALF, 0.0f, i - GRID_HALF);
        vec3_set(vertices[(GRID_SIZE + 1) * 2 + (i * 2) + 1], +GRID_HALF, 0.0f, i - GRID_HALF);
    }

    glGenVertexArrays(1, &grid->vao);
    glBindVertexArray(grid->vao);

    // Vertices
    glGenBuffers(1, &grid->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, grid->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);  // Attrib pointer for currently bound buffer
}

void draw_grid(grid_t *grid, int shader) {
    GLint color_loc = glGetUniformLocation(shader, "color");
    glUniform3f(color_loc, 0.5f, 0.5f, 0.5f);

    glBindVertexArray(grid->vao);
    glDrawArrays(GL_LINES, 0, (GRID_SIZE + 1) * 4);
}

void free_grid(grid_t *grid) {
    glDeleteVertexArrays(1, &grid->vao);
    glDeleteBuffers(1, &grid->vbo);
}
