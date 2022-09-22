#ifndef MODEL_H
#define MODEL_H

#include "glfw.h"
#include "linmath.h"

#define RENDER_MODE_VERTICES  0b0001
#define RENDER_MODE_FACES     0b0010
#define RENDER_MODE_NORMALS   0b0100
#define RENDER_MODE_WIREFRAME 0b1000

struct _face_renderer_t;
typedef struct _face_renderer_t face_renderer_t;

struct _vertex_renderer_t;
typedef struct _vertex_renderer_t vertex_renderer_t;

struct _normal_renderer_t;
typedef struct _normal_renderer_t normal_renderer_t;

struct _wireframe_renderer_t;
typedef struct _wireframe_renderer_t wireframe_renderer_t;

struct _face_t 
{
    uint32_t len;
    uint32_t* indices;

    vec3 midpoint;
    vec3 normal;

    uint8_t color_index;
};

typedef struct _face_t face_t;

struct _model_t {
    vec3 palette[8];
    uint8_t palette_len;

    vec3* vertices;
    uint32_t vertices_cap;
    uint32_t vertices_len;

    face_t* faces;
    uint32_t faces_cap;
    uint32_t faces_len;

    face_renderer_t *face_renderer;
    vertex_renderer_t *vertex_renderer;
    normal_renderer_t *normal_renderer;
    wireframe_renderer_t *wireframe_renderer;

    uint8_t render_mode;
};

typedef struct _model_t model_t;

void init_model(model_t *model);
void free_model(model_t *model);

void render_model(model_t *model);

uint32_t add_vertex(model_t *model, vec3 vertex);
void move_vertices(model_t *model, uint32_t *indices, uint32_t len, vec3 delta);
void scale_vertices(model_t *model, uint32_t *indices, uint32_t len, float factor);
void rotate_vertices(model_t *model, uint32_t *indices, uint32_t len, uint8_t axis, float theta);
void duplicate_vertices(model_t *model, uint32_t *indices, uint32_t len);
void remove_vertices(model_t *model, uint32_t *indices, uint32_t len);

face_t *add_face(model_t *model, uint32_t *indices, uint32_t len);
face_t *extend_edge(model_t *model, uint32_t *indices, uint32_t len);
face_t *extend_face(model_t *model, uint32_t *indices, uint32_t len);

face_t *get_face(model_t *model, uint32_t *indices, uint32_t len);
void flip_face(model_t *model, face_t *face);

void set_render_mode(model_t *model, uint8_t mode);
void toggle_render_mode(model_t *model, uint8_t mode);

void load_vertices(model_t *model, vec3 *vertices, uint32_t len);
void load_palette(model_t *model, vec3 *colors, uint8_t len);
face_t *load_face(model_t *model, uint32_t *indices, uint32_t len);

#endif  // MODEL_H
