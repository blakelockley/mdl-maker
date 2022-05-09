#ifndef NORMAL_RENDERER_H
#define NORMAL_RENDERER_H

#include "glfw.h"
#include "model.h"

struct _normal_renderer_t
{
    GLuint shader;
    GLuint vao, vbo[1];
};

typedef struct _normal_renderer_t normal_renderer_t;

void init_normal_renderer(normal_renderer_t *renderer);
void free_normal_renderer(normal_renderer_t *renderer);

void render_model_normals(normal_renderer_t *renderer, model_t *model);

#endif  // NORMAL_RENDERER_H

