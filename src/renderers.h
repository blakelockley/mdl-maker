#ifndef RENDERERS_H
#define RENDERERS_H

#include "glfw.h"
#include "model.h"

struct _renderer_t {
    uint32_t shader;
    uint32_t vao, vbo_len;
    uint32_t *vbo;
};

typedef struct _renderer_t renderer_t;

renderer_t *init_renderer(renderer_t *renderer, uint32_t vbo_len);
void deinit_renderer(renderer_t *renderer);

// Vertex Renderer

renderer_t *init_vertex_renderer(renderer_t *renderer);
void render_model_vertices(renderer_t *renderer, model_t *model);
void render_model_vertices_selection(renderer_t *renderer, model_t *model, uint32_t *indices, uint32_t len);

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

// Debug Renderer (singleton debug renderer instance is owned by debug_renderer.c file)

void init_debug_renderer();

// Control Renderer

renderer_t *init_control_renderer(renderer_t *renderer);
void render_control_point(renderer_t *renderer, vec3 p, vec3 color);
void render_control_line(renderer_t *renderer, vec3 a, vec3 b, vec3 color);
void render_control_plane(renderer_t *renderer, vec3 origin, vec3 normal, float width, float height, vec3 color);

#endif // RENDERERS_H
