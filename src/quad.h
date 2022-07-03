#ifndef QUAD_H
#define QUAD_H

#include "linmath.h"

void init_quad();
void free_quad();

void render_quad(vec2 topLeft, vec2 bottomRight, vec4 color);

#endif  // QUAD_H
