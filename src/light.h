#ifndef LIGHT_H
#define LIGHT_H

#include "glfw.h"
#include "linmath.h"

struct _light_t {
    vec3 pos, color;

    GLuint shader;
    GLuint vao, vbo;
};

typedef struct _light_t light_t;

void init_light();
void draw_light();
void free_light();

void set_light_position(vec3 position);

int check_light_intercept();

#endif  // LIGHT_H