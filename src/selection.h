#ifndef SELECT_H
#define SELECT_H

#include "glfw.h"
#include "linmath.h"
#include "renderers.h"

struct _selection_t {
    bool is_visible;
    
    uint32_t *indices;
    uint32_t len;
    uint32_t cap;
    
    double ax, ay;
    double bx, by;
};

typedef struct _selection_t selection_t;

void init_selection();
void free_selection();

void update_selection();
void render_selection();

#endif  // SELECT_H