#include "axis.h"

#include "linmath.h"
#include "shader.h"

int shader;

void init_axis(axis_t *axis) {
    // clang-format off

    shader = load_shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    
    vec3 vertices[4] = {
        {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}
    };

    u_int32_t edges[3][2] = {
        {0,1}, {0, 2}, {0, 3}
    };
    // clang-format on

    glGenVertexArrays(1, &axis->vao);
    glBindVertexArray(axis->vao);

    // Edges
    glGenBuffers(1, &axis->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, axis->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(edges), edges, GL_STATIC_DRAW);

    // Vertices
    glGenBuffers(1, &axis->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, axis->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);  // Attrib pointer for currently bound buffer
}

void draw_axis(axis_t *axis, int shader, vec3 camera_pos, int width, int height) {
    glUseProgram(shader);
    glViewport(width - 400, height - 400, 400, 400);

    mat4x4 model, view, projection;
    mat4x4_identity(model);

    mat4x4_look_at(view, camera_pos, (vec3){0, 0, 0}, (vec3){0, 1, 0});
    mat4x4_ortho(projection, -1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);

    GLint model_loc = glGetUniformLocation(shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float *)model);

    GLint view_loc = glGetUniformLocation(shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float *)view);

    GLint projection_loc = glGetUniformLocation(shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float *)projection);

    GLint color_loc = glGetUniformLocation(shader, "color");
    glBindVertexArray(axis->vao);

    glUniform3f(color_loc, 1.0f, 0.0f, 0.0f);
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void *)(sizeof(unsigned int) * 0));

    glUniform3f(color_loc, 0.0f, 1.0f, 0.0f);
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void *)(sizeof(unsigned int) * 2));

    glUniform3f(color_loc, 0.0f, 0.0f, 1.0f);
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void *)(sizeof(unsigned int) * 4));

    glViewport(0, 0, width, height);
}

void free_axis(axis_t *axis) {
    glDeleteVertexArrays(1, &axis->vao);
    glDeleteBuffers(1, &axis->vbo);
    glDeleteBuffers(1, &axis->ebo);
}