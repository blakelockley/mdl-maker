#ifndef SELECTION_H
#define SELECTION_H

#include "glfw.h"
#include "linmath.h"
#include "object.h"

struct _selection_t {
    int show_selection;

    double start_x, start_y;

    vec3 tl_pos, tl_dir;
    vec3 br_pos, br_dir;

    GLuint shader;
    GLuint vao, vbo, ebo;
};

typedef struct _selection_t selection_t;

void init_selection();
void draw_selection();
void free_selection();

void set_selection_start(double mouse_x, double mouse_y, int width, int height);
void set_selection_end(double mouse_x, double mouse_y, int width, int height, object_t *object);

#endif  // SELECTION_H