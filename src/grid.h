#ifndef GRID_H
#define GRID_H

#include "glfw.h"
#include "linmath.h"

#define GRID_SIZE 10
#define GRID_HALF (GRID_SIZE / 2.0f)

struct _grid_t {
    GLuint vao, vbo, ebo;
};

typedef struct _grid_t grid_t;

void init_grid(grid_t* grid);
void draw_grid(grid_t* grid, int shader);
void free_grid(grid_t* grid);

#endif  // GRID_H