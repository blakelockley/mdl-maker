#ifndef SELECTION_H
#define SELECTION_H

#include "glfw.h"
#include "linmath.h"
#include "object.h"

struct _selection_t {
    int visible;
    int existing_count;

    double ax, ay;
    double bx, by;

    GLuint shader;
    GLuint vao, vbo;
};

typedef struct _selection_t selection_t;

void init_selection();
void draw_selection();
void free_selection();

void set_selection_start(double x, double y, int include_existing);
void set_selection_end(double x, double y, object_t *object);

#endif  // SELECTION_H