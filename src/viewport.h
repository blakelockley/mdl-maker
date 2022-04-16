#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "glfw.h"
#include "linmath.h"
 
struct _viewport_t {
    int width, height;
};

typedef struct _viewport_t viewport_t;

void get_projection_matrix(mat4x4 m);

#endif  // VIEWPORT_H