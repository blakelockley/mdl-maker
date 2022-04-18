#ifndef SELECT_H
#define SELECT_H

#include "glfw.h"
#include "linmath.h"

struct _select_t {
    int is_visible;

    double ax, ay;
    double bx, by;

    int *selection_buffer;
    int selection_len;
    int selection_cap;
    
    GLuint shader;
    GLuint vao, vbo;
};

typedef struct _select_t select_t;

void init_select();
void draw_select();
void free_select();

void set_select_start(double x, double y);
void set_select_move(double x, double y);
void set_select_end(double x, double y);

#endif  // SELECT_H