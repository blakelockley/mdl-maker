#ifndef MODEL_H
#define CAMERA_H

#include "glfw.h"
#include "linmath.h"

struct _camera_t {
    int shader, third_person;
    vec3 scroll, pos;
    GLuint vao, vbo, ebo;
};

typedef struct _camera_t camera_t;

void init_camera(camera_t* camera);
void draw_camera(camera_t* camera, int shader);
void free_camera(camera_t* camera);

void update_scroll(camera_t* camera, double xoffset, double yoffset);
void update_zoom(camera_t* camera, double delta);

void get_view_matrix(camera_t* camera, mat4x4 view);

#endif  // CAMERA_H