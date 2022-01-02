#include "guide.h"

#include "camera.h"
#include "shader.h"

guide_t guide;
extern camera_t camera;
extern int width, height;

void init_guide() {
    guide.is_visible = 0;

    glGenVertexArrays(1, &guide.vao);
    glBindVertexArray(guide.vao);

    // Vertices
    glGenBuffers(1, &guide.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, guide.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * 2, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);  // Attrib pointer for currently bound buffer

    guide.shader = load_shader("shaders/static.vert", "shaders/static.frag");
}

void draw_guide() {
    if (!guide.is_visible)
        return;

    glUseProgram(guide.shader);

    mat4x4 model, view, projection;
    mat4x4_identity(model);
    mat4x4_look_at(view, camera.pos, camera.dir, camera.up);
    mat4x4_perspective(projection, 45.0f, (float)width / (float)height, 0.1f, 100.0f);

    GLint model_loc = glGetUniformLocation(guide.shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float *)model);

    GLint view_loc = glGetUniformLocation(guide.shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float *)view);

    GLint projection_loc = glGetUniformLocation(guide.shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float *)projection);

    GLint color_loc = glGetUniformLocation(guide.shader, "color");
    glBindVertexArray(guide.vao);

    glUniform3fv(color_loc, 1, (float *)guide.axis);
    glDrawArrays(GL_LINES, 0, 2);
}

void free_guide() {
    glDeleteVertexArrays(1, &guide.vao);
    glDeleteBuffers(1, &guide.vbo);
}

void set_guide(vec3 pos, vec3 axis) {
    guide.is_visible = 1;

    vec3_copy(guide.pos, pos);
    vec3_copy(guide.axis, axis);

    vec3 ray, vertices[2];

    vec3_scale(ray, guide.axis, 5.0f);
    vec3_add(vertices[0], guide.pos, ray);

    vec3_scale(ray, guide.axis, -5.0f);
    vec3_add(vertices[1], guide.pos, ray);

    glBindVertexArray(guide.vao);

    glBindBuffer(GL_ARRAY_BUFFER, guide.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
}
