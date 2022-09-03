#ifndef CAMERA_H
#define CAMERA_H

#include "linmath.h"

struct _camera_t {
    vec3 pos, dir;
    vec3 up, center;

    float zoom;
};

typedef struct _camera_t camera_t;

void init_camera(camera_t *camera);
void free_camera(camera_t *camera);

// Setters
void set_camera_position(camera_t *camera, vec3 v);

// Getters
void get_view_matrix(camera_t *camera, mat4x4 m);
void get_camera_heading(camera_t *camera, vec3 r);

#endif  // CAMERA_H