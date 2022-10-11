#ifndef RENDERERS_H
#define RENDERERS_H

#include "glfw.h"
#include "model.h"

struct _renderer_t
{
    uint32_t shader;
    uint32_t vao, vbo_len;
    uint32_t *vbo;

    void (*render)(struct _renderer_t *renderer, model_t *model);
};

typedef struct _renderer_t renderer_t;

renderer_t *init_renderer(renderer_t *renderer, uint32_t vbo_len);
void deinit_renderer(renderer_t *renderer);

// Vertex Renderer

renderer_t *init_vertex_renderer(renderer_t *renderer);
void render_model_vertices(renderer_t *renderer, model_t *model);

// Edge Renderer

renderer_t *init_edge_renderer(renderer_t *renderer);
void render_model_edges(renderer_t *renderer, model_t *model);
void render_model_edges_selection(renderer_t *renderer, model_t *model, uint32_t *indices, uint32_t len);

// Face Renderer

renderer_t *init_face_renderer(renderer_t *renderer);
void render_model_faces(renderer_t *renderer, model_t *model);

// Normal Renderer

renderer_t *init_normal_renderer(renderer_t *renderer);
void render_model_normals(renderer_t *renderer, model_t *model);

#endif // RENDERERS_H
