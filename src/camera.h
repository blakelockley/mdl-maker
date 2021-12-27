#ifndef CAMERA_H
#define CAMERA_H

#include "glfw.h"
#include "linmath.h"

struct _camera_t {
    float scroll, zoom;
    vec3 pos, dir, right, up;
    vec3 ray_start, ray;
};

typedef struct _camera_t camera_t;

void init_camera();

void set_scroll(double angle);
void update_scroll(double delta);

void set_zoom(double angle);
void update_zoom(double delta);

void set_ray(double mouse_x, double mouse_y, int width, int height);

#endif  // CAMERA_H