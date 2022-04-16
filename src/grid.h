#ifndef GRID_H
#define GRID_H

#include "glfw.h"
#include "linmath.h"

struct _grid_t {
    GLuint shader;
    GLuint vao, vbo;
};

typedef struct _grid_t grid_t;

void init_grid();
void draw_grid();
void free_grid();

#endif  // GRID_H