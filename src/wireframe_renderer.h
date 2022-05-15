#ifndef WIREFRAME_RENDERER_H
#define WIREFRAME_RENDERER_H

#include "glfw.h"
#include "model.h"

struct _wireframe_renderer_t
{
    GLuint shader;
    GLuint vao, vbo[1];
};

typedef struct _wireframe_renderer_t wireframe_renderer_t;

void init_wireframe_renderer(wireframe_renderer_t *renderer);
void free_wireframe_renderer(wireframe_renderer_t *renderer);

void render_model_wireframe(wireframe_renderer_t *renderer, model_t *model);

#endif  // WIREFRAME_RENDERER_H
