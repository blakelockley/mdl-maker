#ifndef MODEL_H
#define MODEL_H

#include "glfw.h"
#include "linmath.h"

struct _model_t {
    vec3 color;

    vec3* positions;
    uint32_t positions_cap;
    uint32_t positions_len;

    GLuint pos_shader;
    GLuint pos_vao, pos_vbo;
};

typedef struct _model_t model_t;

void init_model();
void draw_model();
void free_model();

void buffer_model();

void add_position(vec3 position);

#endif  // MODEL_H
