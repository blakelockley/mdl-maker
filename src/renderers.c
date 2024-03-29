#include "renderers.h"
#include "stdlib.h"

renderer_t * init_renderer(renderer_t *renderer, uint32_t vbo_len) {
    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    renderer->vbo_len = vbo_len;
    renderer->vbo = malloc(vbo_len * sizeof(uint32_t));

    glGenBuffers(vbo_len, renderer->vbo);

    return renderer;
}

void deinit_renderer(renderer_t *renderer) {
    glDeleteVertexArrays(1, &renderer->vao);
    glDeleteBuffers(renderer->vbo_len, renderer->vbo);
    
    free(renderer->vbo);
}
