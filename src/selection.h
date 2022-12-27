#ifndef SELECT_H
#define SELECT_H

#include "glfw.h"
#include "linmath.h"

#define MODE_VERTEX 1
#define MODE_FACE   2

struct _selection_t {
    uint32_t *indices;
    uint32_t len;
    uint32_t cap;
    
    bool is_visible;
    double ax, ay;
    double bx, by;

    bool is_coplanar;
 
    GLuint shader;
    GLuint vao, vbo;

    uint8_t mode;
};

typedef struct _selection_t selection_t;

void init_selection(selection_t *select);
void free_selection(selection_t *select);

void render_selection(selection_t *select);

void clear_selection(selection_t *select);
void extend_selection(selection_t *select, uint32_t index);

void handle_selection_start(selection_t *selection, float x, float y);
void handle_selection_move(selection_t *selection, float x, float y);
void handle_selection_end(selection_t *selection, float x, float y);

void update_selection(selection_t *selection);

#endif  // SELECT_H