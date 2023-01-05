#ifndef CAMERA_H
#define CAMERA_H

#include "linmath.h"

#define CAMERA_ZOOM_INITIAL 45

struct _camera_t {
    // properties
    vec3 origin;
    float theta, phi, radius;
    
    // perspective/projection
    float zoom;
    
    // derived
    vec3 pos, dir, up;
};

typedef struct _camera_t camera_t;

void init_camera(camera_t *camera);
void free_camera(camera_t *camera);

// Debug methods
void debug_camera(camera_t *camera);

// Update methods
void update_origin(camera_t *camera, float delta_x, float delta_y);
void update_orbit(camera_t *camera, float delta_x, float delta_y);
void update_radius(camera_t *camera, float delta);

// Matrices
void get_view_matrix(camera_t *camera, mat4x4 m);
void get_projection_matrix(camera_t *camera, mat4x4 m);
void get_view_projection_matrix(camera_t *camera, mat4x4 m);

#endif  // CAMERA_H
