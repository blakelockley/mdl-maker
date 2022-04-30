#ifndef MODEL_H
#define MODEL_H

#include "glfw.h"
#include "linmath.h"

struct _face_t 
{
    uint32_t len;
    uint32_t* indices;

    vec3 midpoint;
    vec3 normal;
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

void init_model(model_t *model);
void free_model(model_t *model);

void render_model(model_t *model);

uint32_t add_vertex(model_t *model, vec3 vertex);
face_t *add_face(model_t *model);

void flip_face(model_t *model);
void extend_face(model_t *model);

// TODO: Remove from interface
void update_faces(model_t *model);

#endif  // MODEL_H
