#ifndef GUIDE_H
#define GUIDE_H

#include "glfw.h"
#include "linmath.h"

struct _guide_t {
    int is_visible;
    vec3 pos, axis;

    GLuint shader;
    GLuint vao, vbo;
};

typedef struct _guide_t guide_t;

void init_guide();
void draw_guide();
void free_guide();

void set_guide(vec3 pos, vec3 axis);

#endif  // GUIDE_H