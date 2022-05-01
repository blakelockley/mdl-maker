#ifndef FACE_RENDERER_H
#define FACE_RENDERER_H

#include "glfw.h"
#include "model.h"

struct _face_renderer_t
{
    GLuint shader;
    GLuint vao, vbo[2];
};

typedef struct _face_renderer_t face_renderer_t;


void init_face_renderer(face_renderer_t *renderer);
void free_face_renderer(face_renderer_t *renderer);

void render_model_faces(face_renderer_t *renderer, model_t *model);

#endif  // FACE_RENDERER_H
