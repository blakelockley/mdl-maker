#include "camera.h"
#include "linmath.h"

#include <math.h> 

void init_camera(camera_t *camera) {
    vec3_set(camera->center, 0.0, 0.0, 0.0);
    vec3_set(camera->up, 0.0, 1.0, 0.0);
    
    set_camera_position(camera, (vec3){0.0, 1.0f, 2.0f});
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

int get_camera_direction(camera_t *camera) {
    float x = camera->dir[0];
    float z = camera->dir[2];
    
    if (fabs(x) >= fabs(z)) {
        if (x >= 0)
            return DIRECTION_POS_X;
        else
            return DIRECTION_NEG_X;
    } else {
        if (z >= 0)
            return DIRECTION_POS_Z;
        else
            return DIRECTION_NEG_Z;
    }

    return -1;
}