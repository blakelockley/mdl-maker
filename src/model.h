#ifndef MODEL_H
#define MODEL_H

#include "glfw.h"
#include "linmath.h"

struct _face_renderer_t;
typedef struct _face_renderer_t face_renderer_t;

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

    face_renderer_t *face_renderer;

    GLuint pos_shader;
    GLuint pos_vao, pos_vbo;
};

typedef struct _model_t model_t;

void init_model(model_t *model);
void free_model(model_t *model);

void render_model(model_t *model);

uint32_t add_vertex(model_t *model, vec3 vertex);
void move_vertices(model_t *model, uint32_t *indices, uint32_t len, vec3 delta);
void remove_vertices(model_t *model, uint32_t *indices, uint32_t len);

face_t *add_face(model_t *model, uint32_t *indices, uint32_t len);
face_t *extend_edge(model_t *model, uint32_t *indices, uint32_t len);

face_t *get_face(model_t *model, uint32_t *indices, uint32_t len);
void flip_face(model_t *model, face_t *face);

#endif  // MODEL_H
