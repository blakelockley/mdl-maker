#ifndef RENDERERS_H
#define RENDERERS_H

#include "glfw.h"
#include "linmath.h"

struct _renderer_t {
    uint32_t shader;
    uint32_t vao, vbo_len;
    uint32_t *vbo;
};

typedef struct _renderer_t renderer_t;

void init_renderers();

// Base renderer
// TODO: Rename to base renderer

renderer_t *init_renderer(renderer_t *renderer, uint32_t vbo_len);
void deinit_renderer(renderer_t *renderer);

// Static renderer

void init_static_renderer();
void render_plane(vec3 origin, vec3 normal, float width, float height, vec4 color);

#endif // RENDERERS_H
