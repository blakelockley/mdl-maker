#include "viewport.h"

viewport_t viewport;

void get_projection_matrix(mat4x4 m) {
    mat4x4_perspective(m, 45.0f, (float)viewport.width / (float)viewport.height, 0.1f, 100.0f);
}
