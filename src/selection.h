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

struct _selection_t {
    selection_state_t state;
    
    bool is_hovering;
    bool is_hovering_handle;
    bool is_hovering_rotation[3];
    
    uint32_t *indices;
    uint32_t len;
    uint32_t cap;

    uint32_t *faces;
    uint32_t faces_len;
    uint32_t faces_cap;
    
    double ax, ay;
    double bx, by;
    
    double resize_x, resize_y;

    int rotation_axis;
    float rotations[3];
    float initial_rotation, initial_angle;
    
    bool is_coplanar;
    vec3 midpoint, offset, normal;

    renderer_t *selection_renderer, *control_renderer, *vertex_renderer, *edge_renderer;
};

typedef struct _selection_t selection_t;

void init_selection(renderer_t *selection_renderer, renderer_t *control_renderer, renderer_t *vertex_renderer, renderer_t *edge_renderer);
void free_selection();

void update_selection();
void render_selection();

void clear_selection();
void extend_selection();

#endif  // SELECT_H