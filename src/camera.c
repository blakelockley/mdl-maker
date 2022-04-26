#include "camera.h"
#include "linmath.h"

void init_camera(camera_t *camera) {
    vec3_set(camera->pos, 0.0, 1.0f, 2.0f);
    vec3_set(camera->dir, 0.0, -1.0, -1.0);
    
    vec3_set(camera->up, 0.0, 1.0, 0.0);
    vec3_set(camera->right, 1.0, 0.0, 0.0);
    
    vec3_set(camera->center, 0.0, 0.0, 0.0);
}

void free_camera(camera_t *camera) {
    // no-op
}

// Setters

void set_camera_position(camera_t *camera, vec3 pos) {
    vec3_copy(camera->pos, pos);

    vec3_sub(camera->dir, camera->center, camera->pos);
    vec3_normalize(camera->dir, camera->dir);

    vec3_cross(camera->right, camera->dir, camera->up);
    vec3_normalize(camera->right, camera->right);
}

// Getters

void get_view_matrix(camera_t *camera, mat4x4 m) { 
    mat4x4_look_at(m, camera->pos, camera->center, camera->up);
}
