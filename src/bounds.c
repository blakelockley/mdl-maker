#include "bounds.h"

#include "camera.h"
#include "shader.h"

bounds_t bounds;
extern camera_t camera;
extern int width, height;

void init_bounds() {
    // clang-format off
    vec3 vertices[] = {
        {-0.5f, -0.5f, -0.5f},
        {+0.5f, -0.5f, -0.5f},
        {+0.5f, -0.5f, +0.5f},
        {-0.5f, -0.5f, +0.5f},
        {-0.5f, +0.5f, -0.5f},
        {+0.5f, +0.5f, -0.5f},
        {+0.5f, +0.5f, +0.5f},
        {-0.5f, +0.5f, +0.5f},
    };  

    uint32_t indices[] = {
        0, 1, 1, 2, 2, 3, 3, 0,
        0, 4, 1, 5, 2, 6, 3, 7,
        4, 5, 5, 6, 6, 7, 7, 4,
    };
    // clang-format on

    glGenVertexArrays(1, &bounds.vao);
    glBindVertexArray(bounds.vao);

    // Vertices
    glGenBuffers(1, &bounds.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, bounds.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Indices
    glGenBuffers(1, &bounds.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bounds.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);  // Attrib pointer for currently bound buffer

    bounds.shader = load_shader("shaders/static.vert", "shaders/static.frag");
}

void draw_bounds() {
    glUseProgram(bounds.shader);

    mat4x4 model, view, projection;
    mat4x4_identity(model);
    mat4x4_look_at(view, camera.pos, camera.dir, camera.up);
    mat4x4_perspective(projection, 45.0f, (float)width / (float)height, 0.1f, 100.0f);

    GLint model_loc = glGetUniformLocation(bounds.shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float *)model);

    GLint view_loc = glGetUniformLocation(bounds.shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float *)view);

    GLint projection_loc = glGetUniformLocation(bounds.shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float *)projection);

    GLint color_loc = glGetUniformLocation(bounds.shader, "color");
    glUniform3f(color_loc, 0.75f, 0.45f, 0.15f);

    glBindVertexArray(bounds.vao);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
}

void free_bounds() {
    glDeleteVertexArrays(1, &bounds.vao);
    glDeleteBuffers(1, &bounds.vbo);
}
