#ifndef AXIS_H
#define AXIS_H

#include "glfw.h"
#include "linmath.h"

struct _axis_t {
    GLuint shader;
    GLuint vao, vbo, ebo;
};

typedef struct _axis_t axis_t;

void init_axis();
void draw_axis();
void free_axis();

#endif  // AXIS_H