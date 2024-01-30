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

void init_camera();
void free_camera();

// Update methods
void update_camera_origin(float delta_x, float delta_y);
void update_camera_orbit(float delta_x, float delta_y);
void update_camera_radius(float delta);

// Matrices
void get_view_matrix(mat4x4 m);
void get_projection_matrix(mat4x4 m);
void get_view_projection_matrix(mat4x4 m);

#endif  // CAMERA_H
