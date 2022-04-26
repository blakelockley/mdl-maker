#ifndef GRID_H
#define GRID_H

#include "glfw.h"
#include "linmath.h"

struct _grid_t {
    GLuint vao, vbo; 
    GLuint shader;
};

typedef struct _grid_t grid_t;

void init_grid(grid_t *grid);
void free_grid(grid_t *grid);

void render_grid(grid_t *grid);

#endif  // GRID_H