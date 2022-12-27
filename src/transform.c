#include <stdlib.h>
#include "transform.h"

void calculate_transform_midpoint(vec3 result, model_t *model, selection_t *selection) {
    vec3_zero(result);
    
    for (int i = 0; i < selection->len; i++)
        vec3_add(result, result, model->vertices[selection->indices[i]]);
    
    vec3_scale(result, result, 1.0f / (float)selection->len);
}

void init_transform(transform_t *transform) {
    transform->initial_vertices_cap = 10;
    transform->initial_vertices = malloc(sizeof(vec3) * transform->initial_vertices_cap);
}

void start_transform(transform_t *transform, model_t *model, selection_t *selection) {
    transform->model = model;
    transform->selection = selection;
    
    calculate_transform_midpoint(transform->midpoint, model, selection);

    for (int i = 0; i < selection->len; i++) {
        if (selection->len == transform->initial_vertices_cap) {
            transform->initial_vertices_cap *= 2;
            transform->initial_vertices = (vec3*)realloc(transform->initial_vertices, sizeof(vec3) * transform->initial_vertices_cap);
        }

        vec3_copy(transform->initial_vertices[i], model->vertices[selection->indices[i]]);
    }

    vec3_set(transform->translation_axis, 0.0f, 1.0f, 0.0f);
    transform->translation_delta = 0.0f;

    vec3_set(transform->rotation_axis, 1.0f, 0.0f, 0.0f);
    transform->rotation_delta = 0.0f;

    transform->scale = 1.0f;
}

void scale(transform_t *transform) {
    model_t *model = transform->model;
    selection_t *selection = transform->selection;
    
    for (int i = 0; i < selection->len; i++) {
        vec3 delta;
        vec3_sub(delta, transform->initial_vertices[i], transform->midpoint);
        vec3_scale(delta, delta, transform->scale);
        
        vec3_add(model->vertices[selection->indices[i]], transform->midpoint, delta);
    }
}

void rotate(transform_t *transform) {
    // model_t *model = transform->model;
    // selection_t *selection = transform->selection;
    
    // for (int i = 0; i < selection->len; i++) {
    //     vec3 delta;
    //     vec3_sub(delta, transform->initial_vertices[i], transform->midpoint);
    //     vec3_scale(delta, delta, transform->scale_delta);
        
    //     vec3_add(model->vertices[selection->indices[i]], transform->midpoint, delta);
    // }
}


void translate(transform_t *transform) {
    vec3 translation;
    vec3_scale(translation, transform->translation_axis, transform->translation_delta);

    model_t *model = transform->model;
    selection_t *selection = transform->selection;
    
    for (int i = 0; i < selection->len; i++) {
        uint32_t index = selection->indices[i];
        vec3_add(model->vertices[index], model->vertices[index], translation);
    }
}

void apply_transform(transform_t *transform) {
    scale(transform);
    rotate(transform);
    translate(transform);
}

void reset_transform(transform_t *transform) {
    transform->scale = 1.0f;
    transform->rotation_delta = 0.0f;
    transform->translation_delta = 0.0f;
}

void start_extend(transform_t *transform) {
    selection_t *selection = transform->selection;
    
    uint32_t sorted_indices[selection->len];
    uint32_t extend_indices[selection->len];

    vec3 normal;
    calculate_normal(transform->model, normal, selection->indices, selection->len);

    for (int i = 0; i < selection->len; i++)
        sorted_indices[i] = selection->indices[i];

    sort_by_angle(transform->model, transform->midpoint, normal, sorted_indices, selection->len);
    
    for (int i = 0; i < selection->len; i++) {
        vec3 v;
        vec3_add(v, transform->model->vertices[sorted_indices[i]], normal);

        extend_indices[i] = add_vertex(transform->model, v);
    }

    for (int i = 0; i < selection->len; i++) {
        uint32_t ia = sorted_indices[i];
        uint32_t ib = sorted_indices[(i + 1) % selection->len];
        uint32_t ic = extend_indices[(i + 1) % selection->len];
        uint32_t id = extend_indices[i];
        
        uint32_t face_index = add_face_quad(transform->model, ia, ib, ic, id);
        vec3_set(transform->model->faces[face_index].color, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f);
    }
}

void flip_extend(transform_t *transform) {
    // TODO: Add ability to flip extend normal
}