#ifndef SELECTION_H
#define SELECTION_H

#include "glfw.h"
#include "linmath.h"
#include "object.h"

struct _selection_t {
    int visible;

    double ax, ay;
    double bx, by;

    GLuint shader;
    GLuint vao, vbo;
};

typedef struct _selection_t selection_t;

void init_selection();
void draw_selection();
void free_selection();

void set_selection_start(double mouse_x, double mouse_y, int width, int height);
void set_selection_end(double mouse_x, double mouse_y, int width, int height, object_t *object);

#endif  // SELECTION_H