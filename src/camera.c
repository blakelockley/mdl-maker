#include "camera.h"
#include "linmath.h"
#include "macros.h"

#include <math.h>

void init_camera(camera_t *camera) {
    vec3_set(camera->up, 0.0f, 1.0f, 0.0f);

    vec3_set(camera->pos, 0.0f, 1.0f, 2.0f);
    vec3_scale(camera->dir, camera->pos, -1.0f);
    vec3_normalize(camera->dir, camera->dir);

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

void update_position(camera_t *camera, float delta_x, float delta_y) {
    // xz
    vec3 right;
    vec3_cross(right, camera->dir, camera->up);
    vec3_normalize(right, right);
    
    vec3 xz_vector;
    vec3_scale(xz_vector, right, delta_x * -0.01f);
    vec3_add(camera->pos, camera->pos, xz_vector);

    // y
    camera->pos[1] += delta_y * 0.01f;
}

void update_orbit(camera_t *camera, float delta_x, float delta_y) {
    // pos -> orbit 
    
    // radius
    float r = vec3_len(camera->pos);
    
    // xz -> theta
    float theta = atan2(camera->pos[2], camera->pos[0]);
    theta += delta_x * 0.01f;

    // y -> phi
    float phi   = asinf(camera->pos[1] / r);
    phi += delta_y * 0.01f;
    phi = CLAMP(phi, (-M_PI / 2) + 0.1f, (M_PI / 2) - 0.1f);    

    // xz
    camera->pos[0] = cosf(phi) * cosf(theta) * r;
    camera->pos[2] = cosf(phi) * sinf(theta) * r;

    // y
    camera->pos[1] = sinf(phi) * r;

    // dir
    vec3_scale(camera->dir, camera->pos, -1.0f);
    vec3_normalize(camera->dir, camera->dir);
}

void update_depth(camera_t *camera, float delta) {
    delta = delta * -0.1f;
    
    vec3 vector;
    vec3_scale(vector, camera->dir, delta);

    vec3 new_pos;
    vec3_add(new_pos, camera->pos, vector);

    float len = vec3_len(new_pos);
    if (len >= 1.0f)
        vec3_copy(camera->pos, new_pos);
}
