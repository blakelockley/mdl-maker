#ifndef AXIS_H
#define AXIS_H

#include "glfw.h"
#include "linmath.h"

struct _axis_t {
    GLuint vao, vbo, ebo;
};

typedef struct _axis_t axis_t;

void init_axis(axis_t* axis);
void draw_axis(axis_t* axis, int shader, vec3 camera_pos, int width, int height);
void free_axis(axis_t* axis);

#endif  // AXIS_H