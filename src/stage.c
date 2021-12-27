#include "stage.h"

#include "camera.h"

#define STAGE_SLICES 16

stage_t stage;
extern camera_t camera;
extern int width, height;

void init_stage() {
    vec3 vertices[STAGE_SLICES + 2];
    // uint32_t indices[STAGE_SLICES * 3];

    vec3_copy(vertices[0], (vec3){0.0f, 0.0f, 0.0f});

    for (int i = 0; i < STAGE_SLICES + 1; i++) {
        float angle = (float)i / STAGE_SLICES * M_PI * 2;
        vec3_copy(vertices[i + 1], (vec3){cos(angle) * 2, 0.0f, sin(angle) * 2});
    }

    glGenVertexArrays(1, &stage.vao);
    glBindVertexArray(stage.vao);

    // Vertices
    glGenBuffers(1, &stage.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, stage.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // // Indices
    // glGenBuffers(1, stage.ebo);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, stage.ebo);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t), NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);  // Attrib pointer for currently bound buffer
}

void draw_stage(vec3 camera_pos, int shader) {
    glUseProgram(shader);

    mat4x4 model, view, projection;
    mat4x4_identity(model);
    mat4x4_look_at(view, camera.pos, camera.dir, camera.up);
    mat4x4_perspective(projection, 45.0f, (float)width / (float)height, 0.1f, 100.0f);

    GLint model_loc = glGetUniformLocation(shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float *)model);

    GLint view_loc = glGetUniformLocation(shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float *)view);

    GLint projection_loc = glGetUniformLocation(shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float *)projection);

    GLint color_loc = glGetUniformLocation(shader, "color");
    glUniform3f(color_loc, 0.93f, 0.86f, 0.68f);

    glBindVertexArray(stage.vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, STAGE_SLICES + 2);
}

void free_stage() {
    glDeleteVertexArrays(1, &stage.vao);
    glDeleteBuffers(1, &stage.vbo);
}