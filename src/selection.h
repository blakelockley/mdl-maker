#ifndef SELECTION_H
#define SELECTION_H

#include "glfw.h"
#include "linmath.h"

struct _selection_t {
    int show_selection;

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
void set_selection_end(double mouse_x, double mouse_y, int width, int height);

#endif  // SELECTION_H