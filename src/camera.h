#ifndef CAMERA_H
#define CAMERA_H

#include "glfw.h"
#include "linmath.h"

struct _camera_t {
    vec3 pos, dir;
    vec3 up, right;
};

typedef struct _camera_t camera_t;

void init_camera();
void get_view_matrix(mat4x4 m);

#endif  // CAMERA_H