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

    GLuint vao, vbo, ebo;
};

typedef struct _model_t model_t;

void init_model(model_t* model);
void draw_model(model_t* model, int* indices, int index_len, int shader);
void free_model(model_t* model);

void add_vertex(model_t* model, vec3 vertex);
void move_vertex(model_t* model, int index, vec3 vertex);
void add_face(model_t* model, int* index_buffer, int index_len);

int find_intercept(model_t* model, camera_t* camera);

#endif  // MODEL_H