#ifndef CAMERA_H
#define CAMERA_H

#include "linmath.h"

#define CAMERA_ZOOM_INITIAL 45

struct _camera_t {
    vec3 pos, dir, up;
    float zoom;
};

typedef struct _camera_t camera_t;

void init_camera(camera_t *camera);
void free_camera(camera_t *camera);

// Update methods
void update_position(camera_t *camera, float delta_x, float delta_y);
void update_orbit(camera_t *camera, float delta_x, float delta_y);
void update_depth(camera_t *camera, float delta);

// Matrices
void get_view_matrix(camera_t *camera, mat4x4 m);

#endif  // CAMERA_H

// TODO: Merge this with viewport so camera is soley responsible for matrcies
//   OR: Have a new file/struct responsible for matrices