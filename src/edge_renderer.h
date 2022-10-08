#ifndef edge_RENDERER_H
#define edge_RENDERER_H

#include "glfw.h"
#include "model.h"

struct _edge_renderer_t
{
    GLuint shader;
    GLuint vao, vbo[1];
};

typedef struct _edge_renderer_t edge_renderer_t;

void init_edge_renderer(edge_renderer_t *renderer);
void free_edge_renderer(edge_renderer_t *renderer);

void render_model_edges(edge_renderer_t *renderer, model_t *model);

#endif  // edge_RENDERER_H
