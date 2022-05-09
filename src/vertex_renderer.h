#ifndef VERTEX_RENDERER_H
#define VERTEX_RENDERER_H

#include "glfw.h"
#include "model.h"

struct _vertex_renderer_t
{
    GLuint shader;
    GLuint vao, vbo[1];
};

typedef struct _vertex_renderer_t vertex_renderer_t;

void init_vertex_renderer(vertex_renderer_t *renderer);
void free_vertex_renderer(vertex_renderer_t *renderer);

void render_model_vertices(vertex_renderer_t *renderer, model_t *model);

#endif  // VERTEX_RENDERER_H
