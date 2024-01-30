#include "scene.h"

extern scene_t *scene;

void set_ground_color(vec3 color) {
    vec3_copy(scene->ground_color, color);
}
