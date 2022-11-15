#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "linmath.h"
#include "model.h"
#include "selection.h"

// TODO: Currently unused
#define AXIS_NONE 0
#define AXIS_X    1
#define AXIS_Y    2
#define AXIS_Z    3

struct _transform_t
{
    model_t *model;
    selection_t *selection;
    
    vec3 midpoint;
    
    vec3 *deltas;
    uint32_t deltas_len;
    uint32_t deltas_cap;
    
    vec3 translation_axis;
    float translation_delta;
    
    vec3 rotation_axis;
    float rotation_delta;
    
    vec3 scale_axis;
    float scale_delta;

};

typedef struct _transform_t transform_t;

void init_transform(transform_t *transform);

void start_transform(transform_t *transform, model_t *model, selection_t *selection);
void apply_transform(transform_t *transform);

#endif // TRANSFORM_H