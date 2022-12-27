#ifndef MODEL_H
#define MODEL_H

#include "glfw.h"
#include "linmath.h"
 
#define INDEX_NOT_FOUND ((uint32_t) -1)

struct _face_t 
{
    uint32_t len;
    uint32_t* indices;

    vec3 normal;
    vec3 color;
    
    vec3 midpoint;
};

typedef struct _face_t face_t;

struct _model_t {
    char *filename;

    vec3* vertices;
    uint32_t vertices_cap;
    uint32_t vertices_len;

    face_t* faces;
    uint32_t faces_cap;
    uint32_t faces_len;
};

typedef struct _model_t model_t;

// Model

void init_model(model_t *model);
void free_model(model_t *model);

// Vertices

uint32_t add_vertex(model_t *model, vec3 vertex);
void update_vertex(model_t *model, uint32_t index, vec3 vertex);
void remove_vertex(model_t *model, uint32_t index);

// Face

uint32_t add_face(model_t *model, uint32_t *indices, uint32_t len);
uint32_t add_face_tri(model_t *model, uint32_t ia, uint32_t ib, uint32_t ic);
uint32_t add_face_quad(model_t *model, uint32_t ia, uint32_t ib, uint32_t ic, uint32_t id);

void flip_face(model_t *model, uint32_t index);
void set_face_color(model_t *model, uint32_t index, vec3 color);

void remove_face(model_t *model, uint32_t index);

// Helper

void calculate_normal(model_t *model, vec3 normal, uint32_t *indices, uint32_t len);
void calculate_midpoint(model_t *model, vec3 midpoint, uint32_t *indices, uint32_t len);
void recalculate_faces(model_t *model);

int check_coplanar_vertices(model_t *model, uint32_t *indices, uint32_t len);
void sort_by_angle(model_t *model, vec3 midpoint, vec3 normal, uint32_t *indices, uint32_t len);

#endif  // MODEL_H
