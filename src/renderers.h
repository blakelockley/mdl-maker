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

renderer_t *init_vertex_renderer();
void render_model_vertices(model_t *model);
void render_model_vertices_selection(model_t *model, uint32_t *indices, uint32_t len);
void deinit_vertex_renderer();

// Edge Renderer

renderer_t *init_edge_renderer();
void render_model_edges(model_t *model);
void render_model_edges_selection(model_t *model, uint32_t *indices, uint32_t len);
void deinit_edge_renderer();

// Face Renderer

renderer_t *init_face_renderer();
void render_model_faces(model_t *model);
void deinit_face_renderer();

// Normal Renderer

renderer_t *init_normal_renderer();
void render_model_normals(model_t *model);
void deinit_normal_renderer();

// Selection Renderer

renderer_t *init_selection_renderer();
void render_selection_box(float ax, float ay, float bx, float by, vec3 color);
void render_selection_handle(float x, float y, float size, vec3 color);
void deinit_selection_renderer();

// Control Renderer

void init_control_renderer();
void render_control_point(vec3 p, float size, vec4 color);
void render_control_line(vec3 a, vec3 b, vec4 color);
void render_control_plane(vec3 origin, vec3 normal, float width, float height, vec4 color);
void render_control_circle(vec3 origin, vec3 normal, float radius, vec4 color);
void deinit_control_renderer();

#endif // RENDERERS_H
