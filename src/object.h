#ifndef MODEL_H
#define MODEL_H

#include "glfw.h"
#include "linmath.h"

struct _vertex_t {
    vec3 position, normal;
};

typedef struct _vertex_t vertex_t;

struct _object_t {
    vec3* positions;
    uint32_t positions_cap;
    uint32_t positions_len;

    uint32_t* indices;
    uint32_t indices_cap;
    uint32_t indices_len;

    vertex_t* vertices;
    uint32_t vertices_cap;
    uint32_t vertices_len;

    GLuint pos_shader;
    GLuint pos_vao, pos_vbo;

    GLuint obj_shader;
    GLuint obj_vao, obj_vbo;
};

typedef struct _object_t object_t;

void init_object(object_t* object);
void draw_object(object_t* object);
void free_object(object_t* object);
void buffer_object(object_t* object);

// Update data methods
void add_position(object_t* object, vec3 position);
void add_index(object_t* object, uint32_t index);
void add_vertex(object_t* object, vertex_t vertex);

// Selection methods
void add_point_selection(object_t* object);
void remove_selection(object_t* object);
void move_selection(object_t* object, vec3 delta);
void position_selection(object_t* object, vec3 origin);
void add_face_selection(object_t* object);

// Ray methods
int find_intercept(object_t* object);

#endif  // MODEL_H