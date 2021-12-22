#ifndef MODEL_H
#define MODEL_H

#include "glfw.h"
#include "linmath.h"

struct _model_t {
    vec3* vertices;
    uint32_t vertices_cap;
    uint32_t vertices_len;

    GLuint vao, vbo, ebo;
};

typedef struct _model_t model_t;

void init_model(model_t* model);
void add_vertex(model_t* model, vec3 vertex);
void draw_model(model_t* model, int shader);
void free_model(model_t* model);

#endif  // MODEL_H