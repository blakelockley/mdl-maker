#ifndef LIGHT_H
#define LIGHT_H

#include "linmath.h"

struct _light_t {
    vec3 pos;
    vec3 color;
};

typedef struct _light_t light_t;

void init_light(light_t *light);
void free_light(light_t *light);

#endif  // LIGHT_H