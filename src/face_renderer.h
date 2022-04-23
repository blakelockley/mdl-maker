#ifndef FACE_RENDERER_H
#define FACE_RENDERER_H

#include "glfw.h"
#include "model.h"

struct _face_renderer_t
{
    GLuint shader;
    GLuint vao, vbo;
};

typedef struct _face_renderer_t face_renderer_t;

void init_face_renderer();
void render_model_faces(model_t *model);

#endif  // FACE_RENDERER_H
