#ifndef MIRROR_H
#define MIRROR_H

#include "model.h"

void gui_mirror();
void render_mirror_plane();
void render_mirror_faces(model_t *model);

void limit_mirror_source_vertex(vec3 v);

#endif  // MIRROR_H