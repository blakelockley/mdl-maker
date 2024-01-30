#include "scene.h"

extern scene_t *scene;

void set_sky_color(vec3 color) {
    vec3_copy(scene->sky_color, color);
}
