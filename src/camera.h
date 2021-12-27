#ifndef CAMERA_H
#define CAMERA_H

#include "glfw.h"
#include "linmath.h"

struct _camera_t {
    vec3 scroll, pos, dir;
    vec3 right, up;
    vec3 ray_start, ray;
};

typedef struct _camera_t camera_t;

void init_camera();

void update_scroll(double xoffset, double yoffset);
void update_zoom(double delta);

void set_ray(double mouse_x, double mouse_y, int width, int height);

#endif  // CAMERA_H