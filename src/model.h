#ifndef MODEL_H
#define MODEL_H

#include "camera.h"
#include "glfw.h"
#include "linmath.h"

struct _model_t {
    vec3* vertices;
    uint32_t vertices_cap;
    uint32_t vertices_len;

    uint32_t* indices;
    uint32_t indices_cap;
    uint32_t indices_len;

    GLuint shader;
    GLuint vao, vbo, ebo;
};

typedef struct _model_t model_t;

void init_model(model_t* model);
void draw_model(model_t* model);
void free_model(model_t* model);

void add_face(model_t* model);
void add_vertex(model_t* model, vec3 vertex);
void remove_vertex(model_t* model);

void move_selection(model_t* model, vec3 delta);
void set_selection_position(model_t* model, vec3 origin);

int find_intercept(model_t* model);

#endif  // MODEL_H