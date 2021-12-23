#include "grid.h"

extern int width, height;

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

void draw_grid(grid_t *grid, vec3 camera_pos, int shader) {
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

    glBindVertexArray(grid->vao);
    glDrawArrays(GL_LINES, 0, (GRID_SIZE + 1) * 4);
}

void free_grid(grid_t *grid) {
    glDeleteVertexArrays(1, &grid->vao);
    glDeleteBuffers(1, &grid->vbo);
}
