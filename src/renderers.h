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

// Selection Renderer

renderer_t *init_selection_renderer(renderer_t *renderer);
void render_selection_box(renderer_t *renderer, float ax, float ay, float bx, float by, vec3 color);
void render_selection_handle(renderer_t *renderer, float x, float y, float size, vec3 color);

// Control Renderer

void init_control_renderer();
void render_control_point(vec3 p, float size, vec3 color);
void render_control_line(vec3 a, vec3 b, vec3 color);
void render_control_plane(vec3 origin, vec3 normal, float width, float height, vec3 color);
void render_control_circle(vec3 origin, vec3 normal, float radius, vec3 color);

#endif // RENDERERS_H
