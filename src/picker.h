#ifndef PICKER_H
#define PICKER_H

#include "glfw.h"
#include "linmath.h"

#include "model.h"

struct _picker_t {
    GLuint shader;
    GLuint vao, vbo[2];
};

typedef struct _picker_t picker_t;

void init_picker(picker_t *picker);
void free_picker(picker_t *picker);

uint32_t render_picker_to_face_id(picker_t *picker, model_t *model);

#endif  // PICKER_H