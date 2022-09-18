#include "camera.h"
#include "linmath.h"
#include "macros.h"

#include <math.h>

void init_camera(camera_t *camera) {
    vec3_set(camera->up, 0.0f, 1.0f, 0.0f);

    vec3_set(camera->orbit, 2.0f, 0, M_PI / 4);
    update_orbit(camera, 0, 0);
    
    // vec3_set(camera->pos, 0.0f, 2.0f, -2.0f);
    // vec3_scale(camera->dir, camera->pos, -1.0f);

    // zoom in degrees
    camera->zoom = 45.0f;
}

void free_camera(camera_t *camera) {
    // no-op
}


// Getters

void get_view_matrix(camera_t *camera, mat4x4 m) { 
    vec3 ahead;
    vec3_add(ahead, camera->pos, camera->dir);

    mat4x4_look_at(m, camera->pos, ahead, camera->up);
}

void update_orbit(camera_t *camera, float delta_x, float delta_y) {
    camera->orbit[1] += delta_x * 0.01f;
    
    camera->orbit[2] += delta_y * 0.01f;
    camera->orbit[2] = CLAMP(camera->orbit[2], (-M_PI / 2) + 0.1f, (M_PI / 2) - 0.1f);    

    // orbit
    float r = camera->orbit[0];
    float theta = camera->orbit[1];
    float phi   = camera->orbit[2];

    // xz
    camera->pos[0] = cosf(phi) * cosf(theta) * r;
    camera->pos[2] = cosf(phi) * sinf(theta) * r;

    // y
    camera->pos[1] = sinf(phi) * r;

    // dir
    vec3_scale(camera->dir, camera->pos, -1.0f);
    vec3_normalize(camera->dir, camera->dir);
}