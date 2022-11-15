#include <stdlib.h>
#include "transform.h"

void calculate_transform_midpoint(vec3 result, model_t *model, selection_t *selection) {
    vec3_zero(result);
    
    for (int i = 0; i < selection->len; i++)
        vec3_add(result, result, model->vertices[selection->indices[i]]);
    
    vec3_scale(result, result, 1.0f / (float)selection->len);
}

void init_transform(transform_t *transform) {
    transform->deltas_len = 0;
    transform->deltas_cap = 10;
    transform->deltas = malloc(sizeof(vec3) * transform->deltas_cap);
}

void start_transform(transform_t *transform, model_t *model, selection_t *selection) {
    transform->model = model;
    transform->selection = selection;
    
    calculate_transform_midpoint(transform->midpoint, model, selection);

    transform->deltas_len = 0;
    for (int i = 0; i < selection->len; i++) {
        if (transform->deltas_len == transform->deltas_cap) {
            transform->deltas_cap *= 2;
            transform->deltas = (vec3*)realloc(transform->deltas, sizeof(vec3) * transform->deltas_cap);
        }

        vec3 delta;
        vec3_sub(delta, model->vertices[selection->indices[i]], transform->midpoint);

        vec3_copy(transform->deltas[transform->deltas_len++], delta);
    }

    vec3_set(transform->translation_axis, 0.0f, 1.0f, 0.0f);
    transform->translation_delta = 0.0f;

    vec3_set(transform->rotation_axis, 1.0f, 0.0f, 0.0f);
    transform->rotation_delta = 0.0f;

    vec3_set(transform->scale_axis, 0.0f, 1.0f, 0.0f);
    transform->scale_delta = 1.0f;
}


void translate(transform_t *transform) {
    vec3 translation;
    vec3_scale(translation, transform->translation_axis, transform->translation_delta);

    model_t *model = transform->model;
    selection_t *sel = transform->selection;
    
    for (int i = 0; i < sel->len; i++) {
        vec3 vertex;
        vec3_add(vertex, transform->midpoint, transform->deltas[i]);

        vec3_add(model->vertices[sel->indices[i]], vertex, translation);
    }
}

void rotate(transform_t *transform) {
    vec3 rotation;
    vec3_scale(rotation, transform->rotation_axis, transform->rotation_delta);

    model_t *model = transform->model;
    selection_t *sel = transform->selection;
    
    for (int i = 0; i < sel->len; i++) {
        vec3 vertex;
        vec3_add(vertex, transform->midpoint, transform->deltas[i]);

        vec3_add(model->vertices[sel->indices[i]], vertex, rotation);
    }
}

void scale(transform_t *transform) {
    vec3 scaling;
    vec3_scale(scaling, transform->scale_axis, transform->scale_delta);

    model_t *model = transform->model;
    selection_t *sel = transform->selection;
    
    for (int i = 0; i < sel->len; i++) {
        vec3 delta;
        vec3_scale(delta, transform->deltas[i], transform->scale_delta);
        vec3_add(model->vertices[sel->indices[i]], transform->midpoint, delta);
    }
}


void apply_transform(transform_t *transform) {
    scale(transform);
    rotate(transform);
    translate(transform);
}
