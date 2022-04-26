#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "linmath.h"
 
struct _viewport_t {
    int width, height;
};

typedef struct _viewport_t viewport_t;

// Setters
void set_viewport(viewport_t *viewport, int width, int height);

// Getters
void get_projection_matrix(viewport_t *viewport, mat4x4 m);

#endif  // VIEWPORT_H