#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "linmath.h"
#include "model.h"

void build_plane(model_t *model, vec3 pos, float side, int n_subdivisions);
void build_circle(model_t *model, vec3 pos, float radius, int n_vertices, bool fill);
void build_cube(model_t *model, vec3 pos, float side);
void build_sphere(model_t *model, vec3 pos, float radius, int n_slices, int n_sectors);
void build_icosphere(model_t *model, vec3 pos, float radius, int order);


#endif  // PRIMITIVES_H
