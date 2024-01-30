#ifndef SCENE_H
#define SCENE_H

#include "linmath.h"

struct _scene_t
{
    vec3 sky_color;
    vec3 ground_color;
};

typedef struct _scene_t scene_t;

void set_sky_color(vec3 color);
void set_ground_color(vec3 color);

void render_scene();

#endif  // SCENE_H