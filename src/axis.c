#include "axis.h"

#include "camera.h"
#include "linmath.h"
#include "shader.h"

axis_t axis;
extern camera_t camera;
extern int width, height;

void init_axis() {
    // clang-format off

    vec3 vertices[4] = {
        {0.0f, 0.0f, 0.0f}, {0.5f, 0.0f, 0.0f}, {0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 0.5f}
    };

    u_int32_t indices[3][2] = {
        {0,1}, {0, 2}, {0, 3}
    };
    // clang-format on

    glGenVertexArrays(1, &axis.vao);
    glBindVertexArray(axis.vao);

    // Vertices
    glGenBuffers(1, &axis.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, axis.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Indices
    glGenBuffers(1, &axis.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, axis.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);  // Attrib pointer for currently bound buffer

    axis.shader = load_shader("shaders/static.vert", "shaders/static.frag");
}

void draw_axis() {
    glViewport(width - 200, height - 200, 200, 200);
    glUseProgram(axis.shader);

    mat4x4 model, view, projection;
    mat4x4_identity(model);
    mat4x4_look_at(view, camera.pos, (vec3){0.0f, 0.0f, 0.0f}, camera.up);
    mat4x4_ortho(projection, -1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);

    GLint model_loc = glGetUniformLocation(axis.shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float *)model);

    GLint view_loc = glGetUniformLocation(axis.shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float *)view);

    GLint projection_loc = glGetUniformLocation(axis.shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float *)projection);

    glPointSize(10.0f);
    GLint color_loc = glGetUniformLocation(axis.shader, "color");

    glBindVertexArray(axis.vao);

    glUniform3f(color_loc, 1.0f, 0.0f, 0.0f);
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void *)(sizeof(unsigned int) * 0));
    glDrawArrays(GL_POINTS, 1, 1);

    glUniform3f(color_loc, 0.0f, 1.0f, 0.0f);
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void *)(sizeof(unsigned int) * 2));
    glDrawArrays(GL_POINTS, 2, 1);

    glUniform3f(color_loc, 0.0f, 0.0f, 1.0f);
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void *)(sizeof(unsigned int) * 4));
    glDrawArrays(GL_POINTS, 3, 1);

    glViewport(0, 0, width, height);
}

void free_axis() {
    glDeleteVertexArrays(1, &axis.vao);
    glDeleteBuffers(1, &axis.vbo);
    glDeleteBuffers(1, &axis.ebo);
}
