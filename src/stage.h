#ifndef STAGE_H
#define STAGE_H

#include "glfw.h"
#include "linmath.h"

struct _stage_t {
    vec3 origin;
    GLuint vao, vbo, ebo;
};

typedef struct _stage_t stage_t;

void init_stage();
void draw_stage(vec3 camera_pos, int shader);
void free_stage();

#endif  // STAGE_H