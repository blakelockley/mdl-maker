#include "camera.h"

#include <stdlib.h>

#include "shader.h"

extern int width, height;

void update_camera_position(camera_t*);

void init_camera(camera_t* camera) {
    camera->third_person = 0;

    vec3_set(camera->scroll, 0.0f, 2.0f, 2.0f);
    update_camera_position(camera);

    camera->shader = load_shader("shaders/vertex.glsl", "shaders/fragment.glsl");

    glGenVertexArrays(1, &camera->vao);
    glBindVertexArray(camera->vao);

    uint32_t edges[2] = {0, 1};
    vec3 vertices[2];

    // Edges
    glGenBuffers(1, &camera->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, camera->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(edges), edges, GL_DYNAMIC_DRAW);

    // Vertices
    glGenBuffers(1, &camera->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, camera->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);  // Attrib pointer for currently bound buffer
}

void draw_camera(camera_t* camera) {
    if (!camera->third_person)
        return;

    glUseProgram(camera->shader);

    mat4x4 model, view, projection;
    mat4x4_identity(model);
    mat4x4_perspective(projection, 45.0f, (float)width / (float)height, 0.1f, 100.0f);
    get_view_matrix(camera, view);

    GLint model_loc = glGetUniformLocation(camera->shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)model);

    GLint view_loc = glGetUniformLocation(camera->shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float*)view);

    GLint projection_loc = glGetUniformLocation(camera->shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)projection);

    GLint color_loc = glGetUniformLocation(camera->shader, "color");

    glBindVertexArray(camera->vao);
    glPointSize(40);

    vec3 vertices[3];
    vec3_copy(vertices[0], camera->pos);

    vec3 vector;
    vec3_sub(vector, (vec3){0, 0, 0}, camera->pos);
    vec3_normalize(vector, vector);
    vec3_scale(vector, vector, 0.5f);
    vec3_add(vector, camera->pos, vector);
    vec3_copy(vertices[1], vector);

    glBindBuffer(GL_ARRAY_BUFFER, camera->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glUniform3f(color_loc, 0.45f, 0.75f, 0.95f);
    glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 0));

    glUniform3f(color_loc, 0.25f, 0.35f, 0.25f);
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 0));
}

void free_camera(camera_t* camera) {
    glDeleteVertexArrays(1, &camera->vao);
    glDeleteBuffers(1, &camera->vbo);
    glDeleteBuffers(1, &camera->ebo);
}

void update_camera_position(camera_t* camera) {
    camera->pos[0] = -sinf(camera->scroll[0]) * camera->scroll[2];
    camera->pos[1] = camera->scroll[1];
    camera->pos[2] = cosf(camera->scroll[0]) * camera->scroll[2];
}

void update_scroll(camera_t* camera, double xoffset, double yoffset) {
    camera->scroll[0] += xoffset;
    camera->scroll[1] += yoffset;

    update_camera_position(camera);
}

void update_zoom(camera_t* camera, double delta) {
    camera->scroll[2] += delta;
    camera->scroll[2] = fmin(fmax(camera->scroll[2], 0.5f), 10.0f);

    update_camera_position(camera);
}

void get_view_matrix(camera_t* camera, mat4x4 view) {
    if (camera->third_person)
        mat4x4_look_at(view, (vec3){5, 5, 5}, (vec3){0, 0, 0}, (vec3){0, 1, 0});
    else
        mat4x4_look_at(view, camera->pos, (vec3){0, 0, 0}, (vec3){0, 1, 0});
}
