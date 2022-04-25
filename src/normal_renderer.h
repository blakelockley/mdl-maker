#ifndef NORMAL_RENDERER_H
#define NORMAL_RENDERER_H

#include "glfw.h"
#include "model.h"

struct _normal_renderer_t
{
    GLuint shader;
    GLuint vao, vbo;
};

typedef struct _normal_renderer_t normal_renderer_t;

void init_normal_renderer();
void render_model_normals(model_t *model);

#endif  // NORMAL_RENDERER_H
