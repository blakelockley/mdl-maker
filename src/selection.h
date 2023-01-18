#ifndef SELECT_H
#define SELECT_H

#include "glfw.h"
#include "linmath.h"
#include "renderers.h"

enum _selection_state_t {
    INITIAL   = 0,
    SELECTING = 1,
    SELECTED  = 2,
    MOVING    = 3,
    RESIZING  = 4,
    ROTATING  = 5,
};

typedef enum _selection_state_t selection_state_t;

enum _selection_control_t {
    NONE      = 0,
    SELECTION = 1,
    RESIZE    = 2,
    ROTATE_X  = 3,
    ROTATE_Y  = 4,
    ROTATE_Z  = 5,
};

typedef enum _selection_control_t selection_control_t;

struct _selection_t {
    selection_state_t state;

    selection_control_t hovering_control;
    
    uint32_t *indices;
    uint32_t len;
    uint32_t cap;

    uint32_t *faces;
    uint32_t faces_len;
    uint32_t faces_cap;

    vec3 *deltas; // deltas_len == len
    uint32_t deltas_cap; 
    
    double ax, ay;
    double bx, by;
    
    double resize_x, resize_y;

    int rotation_axis;
    float rotations[3];
    float initial_rotation, initial_angle, last_angle;

    bool show_rotate;
    bool allow_x, allow_y, allow_z;
    
    bool is_coplanar;
    vec3 midpoint, offset, normal;

    renderer_t *selection_renderer, *control_renderer, *vertex_renderer, *edge_renderer;
};

typedef struct _selection_t selection_t;

void init_selection(renderer_t *selection_renderer, renderer_t *control_renderer, renderer_t *vertex_renderer, renderer_t *edge_renderer);
void free_selection();

void update_selection();
void render_selection();

void select_all();
void clear_selection();

#endif  // SELECT_H