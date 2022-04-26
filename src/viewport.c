#include "viewport.h"

void set_viewport(viewport_t *viewport, int width, int height) {
    viewport->width = width;
    viewport->height = height;
}

// Getters

void get_projection_matrix(viewport_t *viewport, mat4x4 m) {
    mat4x4_perspective(m, 45.0f, (float)viewport->width / (float)viewport->height, 0.1f, 100.0f);
}
