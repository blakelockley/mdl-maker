#ifndef SELECT_H
#define SELECT_H

#include "glfw.h"
#include "linmath.h"

struct _selection_t {
    uint32_t *indices;
    uint32_t len;
    uint32_t cap;
    
    int is_visible;
    double ax, ay;
    double bx, by;
 
    GLuint shader;
    GLuint vao, vbo;
};

typedef struct _selection_t selection_t;

void init_selection(selection_t *select);
void free_selection(selection_t *select);

void render_selection(selection_t *select);

void clear_selection(selection_t *select);
void extend_selection(selection_t *select, uint32_t index);

void handle_selection_start(selection_t *select, double x, double y);
void handle_selection_move(selection_t *select, double x, double y);
void handle_selection_end(selection_t *select, double x, double y, int extend_selection);

#endif  // SELECT_H