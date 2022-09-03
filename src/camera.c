#include "camera.h"
#include "linmath.h"

#include <math.h>

void init_camera(camera_t *camera) {
    vec3_set(camera->center, 0.0, 0.0, 0.0);
    vec3_set(camera->up, 0.0, 1.0, 0.0);
    
    set_camera_position(camera, (vec3){0.0, 1.0f, 2.0f});

    camera->zoom = 45;
}

void free_camera(camera_t *camera) {
    // no-op
}

// Setters

void set_camera_position(camera_t *camera, vec3 pos) {
    vec3_copy(camera->pos, pos);

    vec3_sub(camera->dir, camera->center, camera->pos);
    vec3_normalize(camera->dir, camera->dir);
}

// Getters

void get_view_matrix(camera_t *camera, mat4x4 m) { 
    mat4x4_look_at(m, camera->pos, camera->center, camera->up);
}

void get_camera_heading(camera_t *camera, vec3 r) {
    float x = camera->dir[0];
    float z = camera->dir[2];

    if (fabs(x) >= fabs(z))
        vec3_set(r, x / fabs(x), 0.0f, 0.0f);
    else
        vec3_set(r, 0.0f, 0.0f, z / fabs(z));
}