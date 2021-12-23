#include "camera.h"

#include <stdio.h>
#include <stdlib.h>

#define SHOW_RAY 0

extern int width, height;

void update_camera_position(camera_t*);

void init_camera(camera_t* camera) {
    vec3_set(camera->scroll, 0.0f, 2.0f, 2.0f);
    update_camera_position(camera);

    vec3_zero(camera->ray_start);
    vec3_zero(camera->ray);

    glGenVertexArrays(1, &camera->vao);
    glBindVertexArray(camera->vao);

    uint32_t edges[4] = {0, 1, 2, 3};
    vec3 vertices[4];

    // Edges
    glGenBuffers(1, &camera->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, camera->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(edges), edges, GL_DYNAMIC_DRAW);

    // Vertices
    glGenBuffers(1, &camera->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, camera->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);  // Attrib pointer for currently bound buffer
}

void draw_camera(camera_t* camera, int shader) {
    mat4x4 model;
    mat4x4_identity(model);

    GLint model_loc = glGetUniformLocation(shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)model);

    GLint color_loc = glGetUniformLocation(shader, "color");

    glBindVertexArray(camera->vao);
    glPointSize(40);

    vec3 vertices[4];
    vec3_copy(vertices[0], camera->pos);

    vec3 vector;
    vec3_sub(vector, (vec3){0, 0, 0}, camera->pos);
    vec3_normalize(vector, vector);
    vec3_scale(vector, vector, 0.5f);
    vec3_add(vector, camera->pos, vector);
    vec3_copy(vertices[1], vector);

    vec3 ray_end;
    vec3_scale(ray_end, camera->ray, 10.0f);
    vec3_add(ray_end, camera->ray_start, ray_end);

    vec3_copy(vertices[2], camera->ray_start);
    vec3_copy(vertices[3], ray_end);

    glBindBuffer(GL_ARRAY_BUFFER, camera->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glUniform3f(color_loc, 0.45f, 0.75f, 0.95f);
    glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 0));

    glUniform3f(color_loc, 0.25f, 0.35f, 0.25f);
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 0));

    if (SHOW_RAY) {
        glUniform3f(color_loc, 0.0f, 0.0f, 1.0f);
        glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 2));
    }
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

void set_ray(camera_t* camera, double mouse_x, double mouse_y, int width, int height) {
    double normal_x = (2.0f * mouse_x) / width - 1.0f;
    double normal_y = 1.0f - (2.0f * mouse_y) / height;

    vec4 ray_start = (vec4){normal_x, normal_y, -1.0f, 1.0f};
    vec4 ray_end = (vec4){normal_x, normal_y, 0.0f, 1.0f};

    mat4x4 view, projection;
    mat4x4_look_at(view, camera->pos, (vec3){0, 0, 0}, (vec3){0, 1, 0});
    mat4x4_perspective(projection, 45.0f, (float)width / (float)height, 0.1f, 100.0f);

    mat4x4_invert(view, view);
    mat4x4_invert(projection, projection);

    mat4x4_mul_vec4(ray_start, projection, ray_start);
    vec4_scale(ray_start, ray_start, 1.0f / ray_start[3]);

    mat4x4_mul_vec4(ray_start, view, ray_start);
    vec4_scale(ray_start, ray_start, 1.0f / ray_start[3]);

    mat4x4_mul_vec4(ray_end, projection, ray_end);
    vec4_scale(ray_end, ray_end, 1.0f / ray_end[3]);

    mat4x4_mul_vec4(ray_end, view, ray_end);
    vec4_scale(ray_end, ray_end, 1.0f / ray_end[3]);

    vec3 ray_dir;
    vec3_sub(ray_dir, ray_end, ray_start);
    vec3_normalize(ray_dir, ray_dir);

    vec3_copy(camera->ray_start, ray_start);
    vec3_copy(camera->ray, ray_dir);
}