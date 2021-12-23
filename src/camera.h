#ifndef CAMERA_H
#define CAMERA_H

#include "glfw.h"
#include "linmath.h"

struct _camera_t {
    vec3 scroll, pos;
    vec3 ray_start, ray;
    GLuint vao, vbo, ebo;
};

typedef struct _camera_t camera_t;

void init_camera(camera_t* camera);
void draw_camera(camera_t* camera, int shader);
void free_camera(camera_t* camera);

void update_scroll(camera_t* camera, double xoffset, double yoffset);
void update_zoom(camera_t* camera, double delta);

void set_ray(camera_t* camera, double mouse_x, double mouse_y, int width, int height);

#endif  // CAMERA_H