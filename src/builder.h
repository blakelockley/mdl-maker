#ifndef BUILDER_H
#define BUILDER_H

#include "glfw.h"
#include "linmath.h"

struct _builder_t {

};

typedef struct _builder_t builder_t;

void init_builder();
void free_builder();

void update_builder();
void render_builder();


void toggle_building_plane();
void build_vertex(float mouse_x, float mouse_y);

#endif  // BUILDER_H