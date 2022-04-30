#include "light.h"

void init_light(light_t *light) {
    vec3_set(light->color, 1.0f, 1.0f, 1.0f);
    vec3_set(light->pos, 0.0f, 5.0f, 0.0f);
}

void free_light(light_t *light) {
    // no-op
}
