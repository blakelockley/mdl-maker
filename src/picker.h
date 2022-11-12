#ifndef PICKER_H
#define PICKER_H

#include "glfw.h"
#include "linmath.h"

#include "model.h"
#include "selection.h"

// TODO: Move into renderer

struct _picker_t {
    GLuint shader;
    GLuint vao, vbo[2];
};

typedef struct _picker_t picker_t;

void init_picker(picker_t *picker);
void free_picker(picker_t *picker);

void id_to_color(vec3 ret, uint32_t id);

uint32_t render_picker_to_face_id(picker_t *picker, model_t *model);

void render_picker_to_vertex_ids(picker_t *picker, model_t *model);
void select_ids_in_rect(selection_t *selection, vec2 tl, vec2 br);

#endif  // PICKER_H