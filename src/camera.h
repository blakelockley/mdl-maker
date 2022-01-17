#ifndef CAMERA_H
#define CAMERA_H

#include "glfw.h"
#include "linmath.h"

enum camera_view_t {
    CAMERA_VIEW_FORWARD,
    CAMERA_VIEW_TOP,
};

enum camera_projection_t {
    CAMERA_PROJECTION_PERSPECTIVE,
    CAMERA_PROJECTION_ORTHOGRAPHIC,
};

struct _camera_t {
    enum camera_view_t view;
    enum camera_projection_t projection;

    float scroll, angle, zoom;

    vec3 pos, dir, right, up, forward;
    vec3 ray_start, ray;
};

typedef struct _camera_t camera_t;

void init_camera();

void toggle_camera_view();
void toggle_camera_projection();

void set_scroll(double angle);
void update_scroll(double delta);

void set_angle(double angle);
void update_angle(double delta);

void set_zoom(double angle);
void update_zoom(double delta);

void set_ray(double mouse_x, double mouse_y, int width, int height);

void get_projection_matrix(mat4x4 m);

#endif  // CAMERA_H