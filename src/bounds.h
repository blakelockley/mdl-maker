#ifndef BOUNDS_H
#define BOUNDS_H

#include "glfw.h"
#include "linmath.h"

struct _bounds_t {
    GLuint shader;
    GLuint vao, vbo, ebo;
};

typedef struct _bounds_t bounds_t;

void init_bounds();
void draw_bounds();
void free_bounds();

#endif  // BOUNDS_H