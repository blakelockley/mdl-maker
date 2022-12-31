#ifndef SELECT_H
#define SELECT_H

#include "glfw.h"
#include "linmath.h"

enum _selection_mode_t {
    MODE_VERTEX = 0,
    MODE_FACE = 1,
};

typedef enum _selection_mode_t selection_mode_t;

enum _selection_action_t {
    ACTION_SELECT = 0,
    ACTION_MOVE = 1,
};

typedef enum _selection_action_t selection_action_t;

struct _selection_t {
    selection_mode_t mode;
    selection_action_t action;

    uint32_t *indices;
    uint32_t len;
    uint32_t cap;
    
    bool is_visible;
    double ax, ay;
    double bx, by;

    bool is_coplanar;
    vec3 midpoint, offset, normal;

    // TODO: Move this into GUI renderer?
    GLuint shader;
    GLuint vao, vbo;
};

typedef struct _selection_t selection_t;

void init_selection(selection_t *select);
void free_selection(selection_t *select);

void render_selection(selection_t *select);

void clear_selection(selection_t *select);
void append_selection(selection_t *select, uint32_t index);

void handle_selection_start(selection_t *selection, float x, float y);
void handle_selection_move(selection_t *selection, float x, float y);
void handle_selection_end(selection_t *selection, float x, float y);

void update_selection(selection_t *selection);

void move_selection(selection_t *selection);
void scale_selection(selection_t *selection, float scale);
void extend_selection(selection_t *selection);

#endif  // SELECT_H