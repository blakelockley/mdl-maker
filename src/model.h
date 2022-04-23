#ifndef MODEL_H
#define MODEL_H

#include "glfw.h"
#include "linmath.h"

struct _face_t 
{
    uint32_t len;
    uint32_t* indices;
};

typedef struct _face_t face_t;

struct _model_t {
    vec3 color;

    vec3* vertices;
    uint32_t vertices_cap;
    uint32_t vertices_len;

    face_t* faces;
    uint32_t faces_cap;
    uint32_t faces_len;

    GLuint pos_shader;
    GLuint pos_vao, pos_vbo;
};

typedef struct _model_t model_t;

void init_model();
void draw_model();
void free_model();

void buffer_model();

void add_vertex(vec3 vertex);
void add_face();

#endif  // MODEL_H
