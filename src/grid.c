#include "grid.h"

#include "camera.h"
#include "shader.h"

extern camera_t camera;

void init_grid(grid_t *grid) {
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

    glGenVertexArrays(1, &grid->vao);
    glBindVertexArray(grid->vao);

    // Vertices
    glGenBuffers(1, &grid->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, grid->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);  // Attrib pointer for currently bound buffer

    grid->shader = load_shader("shaders/static.vert", "shaders/static.frag");
}

void free_grid(grid_t *grid) {
    glDeleteVertexArrays(1, &grid->vao);
    glDeleteBuffers(1, &grid->vbo);
}

void render_grid(grid_t *grid) {
    glUseProgram(grid->shader);

    mat4x4 mvp;
    get_view_projection_matrix(&camera, mvp);
    
    GLint mvp_loc = glGetUniformLocation(grid->shader, "mvp");
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)mvp);

    GLint color_loc = glGetUniformLocation(grid->shader, "color");
    glBindVertexArray(grid->vao);

    glUniform3f(color_loc, 0.5f, 0.5f, 0.5f);
    glDrawArrays(GL_LINES, 0, 80);

    glUniform3f(color_loc, 1.0f, 0.0f, 0.0f);
    glDrawArrays(GL_LINES, 80, 2);

    glUniform3f(color_loc, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINES, 82, 2);
}
