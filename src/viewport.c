#include "viewport.h"
#include "camera.h"
#include "macros.h"

extern camera_t camera;

void set_viewport(viewport_t *viewport, int width, int height) {
    viewport->width = width;
    viewport->height = height;
}

// Getters

void get_projection_matrix(viewport_t *viewport, mat4x4 m) {
    float fov = CLAMP(camera.zoom, 1, 179) * (M_PI / 180); // Radians
    mat4x4_perspective(m, fov, (float)viewport->width / (float)viewport->height, 0.1f, 100.0f);
}
