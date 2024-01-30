#include "scene.h"
#include "glfw.h"

#include "renderers/renderers.h"

scene_t _scene;
scene_t *scene = &_scene;

void render_scene() {
    glClearColor(scene->sky_color[0], scene->sky_color[1], scene->sky_color[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec4 ground_color_vec4;
    vec4_from_vec3(ground_color_vec4, scene->ground_color, 1.0f);

    render_plane((vec3){0.0, 0.0, 0.0}, (vec3){0.0, 1.0, 0.0}, 100, 100, ground_color_vec4);
}
